/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class ai_pointcloud_demo_depthcamera_MainActivity */

#ifndef _Included_ai_pointcloud_demo_depthcamera_MainActivity
#define _Included_ai_pointcloud_demo_depthcamera_MainActivity
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     ai_pointcloud_demo_depthcamera_MainActivity
 * Method:    DepthEyeSystemNative
 * Signature: (IIILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_DepthEyeSystemNative
  (JNIEnv *env, jobject obj, jint vid, jint pid, jint fd, jstring libPath);

/*
 * Class:     ai_pointcloud_demo_depthcamera_MainActivity
 * Method:    SetModeNative
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_SetModeNative
  (JNIEnv *, jobject, jint mode);

/*
 * Class:     ai_pointcloud_demo_depthcamera_MainActivity
 * Method:    EnableFilterHDRNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_EnableFilterHDRNative
  (JNIEnv *, jobject);

/*
 * Class:     ai_pointcloud_demo_depthcamera_MainActivity
 * Method:    EnableFilterFlyingPixelNative
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_EnableFilterFlyingPixelNative
  (JNIEnv *, jobject, jint threshold);

/*
 * Class:     ai_pointcloud_demo_depthcamera_MainActivity
 * Method:    RegisterRawDataCallbackNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_RegisterRawDataCallbackNative
  (JNIEnv *env, jobject thiz);

/*
 * Class:     ai_pointcloud_demo_depthcamera_MainActivity
 * Method:    RegisterPointCloudCallbackNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_RegisterPointCloudCallbackNative
  (JNIEnv *env, jobject thiz);

/*
 * Class:     ai_pointcloud_demo_depthcamera_MainActivity
 * Method:    OpenCameraNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_OpenCameraNative
  (JNIEnv *, jobject);

/*
 * Class:     ai_pointcloud_demo_depthcamera_MainActivity
 * Method:    GetRevolutionNative
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_GetResolutionNative
  (JNIEnv *env, jobject);

/*
 * Class:     ai_pointcloud_demo_depthcamera_MainActivity
 * Method:    CloseCameraNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_ai_pointcloud_demo_depthcamera_MainActivity_CloseCameraNative
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
