#include <iostream>
#include <jni.h>
#include <android/log.h>
#include <thread>
#include <chrono>
#include <unistd.h>
#include "ai_pointcloud_demo_depthcamera_MainActivity.h"
#include "DepthEyeInterface.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "DEPTH_EYE_NDK", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DEPTH_EYE_NDK", __VA_ARGS__))

using namespace std;

JavaVM *m_vm;
jmethodID m_rawDataCallbackID;
jobject m_obj;

JavaVM *pl_vm;
jmethodID pl_pointCloudbackID;
jobject pl_obj;
PointCloud::DepthEyeSystem *depthEyeSys = NULL;
static int pfd[2];
pthread_t loggingThread;
static const char *LOG_TAG = "DepthEyeJni";

void *loggingFunction(void*) {
    ssize_t readSize;
    char buf[1024];

    while((readSize = read(pfd[0], buf, sizeof buf - 1)) > 0) {
        if(buf[readSize - 1] == '\n') {
            --readSize;
        }
        buf[readSize] = 0;  // add null-terminator
        __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, buf); // Set any log level you want
    }

    return 0;
}

static int runLoggingThread() { // run this function to redirect your output to android log
    setvbuf(stdout, 0, _IOLBF, 0); // make stdout line-buffered
    setvbuf(stderr, 0, _IONBF, 0); // make stderr unbuffered

    /* create the pipe and redirect stdout and stderr */
    pipe(pfd);
    dup2(pfd[1], 1);
    dup2(pfd[1], 2);

    int ret=pthread_create(&loggingThread,NULL,loggingFunction,NULL);

    if( ret == -1) {
        return -1;
    }
    pthread_detach(loggingThread);

    return 0;
}

char* jstringToChar(JNIEnv* env, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

void showFlags(const char* InputArray, jint* OutputArray, int width, int height){
    int saturated = 255;
    int noSaturated = 1;
    for(int i = 0; i < width * height; i++) {
        if ((InputArray[i] & 0x08) == 0x08){
            OutputArray[i] = saturated;
        }else{
            OutputArray[i] = noSaturated;
        }
        OutputArray[i] = OutputArray[i] | OutputArray[i] << 8 | OutputArray[i] << 16 | 255 << 24;
    }
}

void HistogramEq_short(const short* InputArray, jint* OutputArray, int width, int height){
    int max = 0;
    int min = 65536;
    for(int i = 0; i < width * height; i++){
        if(((short*)InputArray)[i] < min){
            min = ((short*)InputArray)[i];
        }
        if(((short*)InputArray)[i] > max){
            max = ((short*)InputArray)[i];
        }
    }
    int span = max - min;
    //prevent division by zero
    if (!span){
        span = 1;
    }
    // fill a temp structure to use to populate the java int array
    for (int i = 0; i < width * height; i++)
    {
        // use min value and span to have values between 0 and 255 (for visualisation)
        OutputArray[i] = (int) ((((short*)InputArray)[i] / (float)span) * 255.0f);
        // set same value for red, green and blue; alpha to 255; to create gray image
        OutputArray[i] = OutputArray[i] | OutputArray[i] << 8 | OutputArray[i] << 16 | 255 << 24;
    }
}

void rawdataCallback(Voxel::DepthCamera &dc, const Voxel::Frame &frame, Voxel::DepthCamera::FrameType c)
{

    const Voxel::ToFRawFrame *d = dynamic_cast<const Voxel::ToFRawFrame *>(&frame);
    if(!d) {
        std::cout << "Null frame captured? or not of type ToFRawFrame" << std::endl;
        return;
    }

    int width = d->size.width;
    int height = d->size.height;
    jint fillPhase[width * height];
    jint fillAmplitude[width * height];
    jint fillFlags[width * height];

    HistogramEq_short((short *)d->phase(), fillPhase, width, height);
    HistogramEq_short((short *)d->amplitude(), fillAmplitude, width, height);
    /*
     * Flag[3]{0 = No pixel saturation 1 = Pixel is saturated}
     * Flag[2]{Reserved. Set to 0.}
     * Flag[1]{Frame counter[1]}
     * Flag[0]{Frame counter[0]}
     * */
    showFlags((char *)d->flags(), fillFlags, width, height);

    // attach to the JavaVM thread and get a JNI interface pointer
    JNIEnv *env;
    m_vm->AttachCurrentThread ( (JNIEnv **) &env, NULL);

    // create java int array
    jintArray intArrayAmplitude = env->NewIntArray (width * height);
    jintArray intArrayPhase = env->NewIntArray (width * height);
    jintArray intArrayFlags = env->NewIntArray (width * height);

    // populate java int array with fill data
    env->SetIntArrayRegion (intArrayPhase, 0, width * height, fillPhase);
    env->SetIntArrayRegion (intArrayAmplitude, 0, width * height, fillAmplitude);
    env->SetIntArrayRegion (intArrayFlags, 0, width * height, fillFlags);

    // call java method and pass amplitude array
    env->CallVoidMethod (m_obj, m_rawDataCallbackID, intArrayAmplitude, intArrayPhase, intArrayFlags);

    // detach from the JavaVM thread
    m_vm->DetachCurrentThread();
}

void pointcloudCallback(Voxel::DepthCamera &dc, const Voxel::Frame &frame, Voxel::DepthCamera::FrameType c)
{
    //todo
}

JNIEXPORT jboolean JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_DepthEyeSystemNative(JNIEnv *env, jobject obj, jint vid, jint pid, jint fd, jstring libPath){
    runLoggingThread();
    LOGI("usb fd from java[%d]", fd);
    const char* pathData = jstringToChar(env, libPath);
    depthEyeSys = new PointCloud::DepthEyeSystem(vid, pid, fd, pathData);
    depthEyeSys->connect();
    bool ret = depthEyeSys->isInitialiszed();
    return ret;
}

JNIEXPORT void JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_SetModeNative(JNIEnv *, jobject, jint mode){
    int stand = 1;
    int precision = 2;
    if(mode == stand){
        depthEyeSys->setMode(PointCloud::DEPTH_MODE::STANDARD);
    }else if (mode == precision){
        depthEyeSys->setMode(PointCloud::DEPTH_MODE::PRICISTION);
    }
}

JNIEXPORT jboolean JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_EnableFilterHDRNative(JNIEnv *, jobject){
    bool ret = depthEyeSys->enableFilterHDR();
    return ret;
}

JNIEXPORT jboolean JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_EnableFilterFlyingPixelNative(JNIEnv *, jobject, jint threshold){
    bool ret = depthEyeSys->enableFilterFlyingPixel(threshold);
    return ret;
}

JNIEXPORT jboolean JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_OpenCameraNative(JNIEnv *, jobject){
    bool ret = depthEyeSys->start();
    return ret;
}

JNIEXPORT jintArray JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_GetResolutionNative(JNIEnv *env, jobject){
    jint Size[2];
    PointCloud::FrameSize frameSize;
    frameSize = depthEyeSys->getRevolution();
    Size[0] = frameSize.width;
    Size[1] = frameSize.height;
    jintArray intArray = env->NewIntArray (2);
    env->SetIntArrayRegion (intArray, 0, 2, Size);
    return intArray;
}

JNIEXPORT void JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_CloseCameraNative(JNIEnv *, jobject){
    bool ret = depthEyeSys->stop();
    if (ret){
        depthEyeSys->disconnect();
    }
}

/**
 *
 * Register Callback && Callback Function
 *
 * **/
JNIEXPORT void JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_RegisterRawDataCallbackNative(JNIEnv *env, jobject thiz){
    // save JavaVM globally; needed later to call Java method in the listener
    env->GetJavaVM(&m_vm);

    m_obj = env->NewGlobalRef(thiz);

    // save refs for callback
    jclass g_class = env->GetObjectClass(m_obj);
    if(g_class == NULL){
        std::cout << "Failed to find class" << std::endl;
    }
    depthEyeSys->registerRawDataCallback(rawdataCallback);
    // save method ID to call the method later in the listener
    m_rawDataCallbackID = env->GetMethodID(g_class, "rawdataCallback", "([I[I[I)V");
}

JNIEXPORT void JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_RegisterPointCloudCallbackNative(JNIEnv *env, jobject thiz){
    // save JavaVM globally; needed later to call Java method in the listener
    env->GetJavaVM(&pl_vm);

    pl_obj = env->NewGlobalRef(thiz);

    // save refs for callback
    jclass g_class = env->GetObjectClass(pl_obj);
    if(g_class == NULL){
        std::cout << "Failed to find class" << std::endl;
    }

    // save method ID to call the method later in the listener
    pl_pointCloudbackID = env->GetMethodID(g_class, "pointCloudCallback", "([F)V");
}
#ifdef __cplusplus
}
#endif
