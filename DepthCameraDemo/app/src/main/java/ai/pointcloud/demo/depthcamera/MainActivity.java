package ai.pointcloud.demo.depthcamera;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import java.util.HashMap;
import java.util.Iterator;

import ai.pointcloud.demo.depthcamera.FileStorageHelper;

import static java.lang.System.out;

public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("deptheye");
        System.loadLibrary("voxel");
        System.loadLibrary("ti3dtof");
        System.loadLibrary("deptheyeh1cdk");
        System.loadLibrary("usb1.0");
    }

    private PendingIntent mUsbPi;
    private UsbManager manager;
    private UsbDeviceConnection usbConnection;
    private Bitmap bmp = null;
    private ImageView amplitudeView;
    boolean m_opened;

    private static final String LOG_TAG = "point_cloud";
    private static final String ACTION_USB_PERMISSION = "ACTION_DEPTH_CAMERA_USB_PERMISSION";

    private static String DepthEyeLibPath;
    private static int DepthEyeCameraFd;
    private int scaleFactor;
    private int[] resolution;

    /*
    * Construct and Init the depth camera
    * param@vid     the usb vendor ID
    * param@pid     the usb product ID
    * param@fd      file descriptor get from the depth camera
    * param@libPath path that store the share objects which need to be load by native code
    * return        true for success, false for failed.
    * */
    public native boolean DepthEyeSystemNative(int vid, int pid, int fd, String libPath);
    /*
    * Setting the video mode of depth camera (OPTION)
    * param@mode    mode for different settings
    *               if mode=1, STANDARD mode, with 30fps and closer range
    *               if mode=2, PRICISTION mode, with 10fps and longer range
    * return        none
    * */
    public native void SetModeNative(int mode); //optional
    /*
    * Enable the HDR filter function
    * param         none
    * return        true for success, false for failed.
    * */
    public native boolean EnableFilterHDRNative();//optional
    /*
    * Enable the FlyingPixel filter function
    * param         none
    * return        true for success, false for failed.
    * */
    public native boolean EnableFilterFlyingPixelNative(int threshold);//optional
    /*
    * Register a raw data callback to native jni code
    * Callback is implemented at following function, and the name was fixed.
    *       public void rawdataCallback(int[] amplitudes,int[] phases,int[] flags)
    *       There are three types of data:
    *           amplitudes for grey level image of the scene
    *           phases for raw depth data of the scene
    *           flags for indicating the over exposed area of the scene
    * */
    public native void RegisterRawDataCallbackNative();  // rawdataCallback
    /*
    * Register a raw data callback to native jni code
    * Callback is implemented at following function, and the name was fixed.
    *       public void pointCloudCallback(float[] XYZPointCloud)
    * Currently is still maintaining...
    * */
    public native void RegisterPointCloudCallbackNative();// pointCloudCallback
    /*
    * open the camera and start streaming
    * return        true for success, false for failed.
    * */
    public native boolean OpenCameraNative();
    /*
    * Get the resolution of current stream
    * return    size 2 int array [width, height]
    * */
    public native int[] GetResolutionNative();
    /*
    * stop streaming and close camera
    * return        true for success, false for failed.
    * */
    public native void CloseCameraNative();


    //broadcast receiver for user usb permission dialog
    private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (ACTION_USB_PERMISSION.equals(action)) {
                UsbDevice device = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);

                if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                    if (device != null) {
                        performUsbPermissionCallback(device);
                        createBitmap();
                    }
                } else {
                    out.println("permission denied for device" + device);
                }
            }
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.d(LOG_TAG, "onCreate()");
        amplitudeView = (ImageView) findViewById(R.id.imageViewAmplitude);
        Resources res = this.getResources();
        amplitudeView.setImageBitmap ( BitmapFactory.decodeResource(res, R.drawable.mars));
        //copy files
        FileStorageHelper.copyFilesFromAssets(this, "conf",
                                "/sdcard/pointcloud_deptheye/conf");
        FileStorageHelper.copyFilesFromAssets(this, "lib",
                "/sdcard/pointcloud_deptheye/lib");
        DepthEyeLibPath = "/sdcard/pointcloud_deptheye";
        Button btnStart = (Button) findViewById(R.id.buttonStart);
        Button btnStop = (Button) findViewById(R.id.buttonStop);

        btnStart.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                openCamera();
            }
        });
        btnStop.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {
                closeCamera();
            }
        });
    }

    @Override
    protected void onDestroy() {
        Log.d(LOG_TAG, "onDestroy()");
        unregisterReceiver(mUsbReceiver);
        if (usbConnection != null) {
            usbConnection.close();
        }
        super.onDestroy();
    }

    public void pointCloudCallback(float[] XYZPointCloud) {
       //to do
    }
    public void rawdataCallback(int[] amplitudes,int[] phases,int[] flags) {
        if (!m_opened) {
            Log.d(LOG_TAG, "Device in Java not initialized");
            return;
        }
        bmp.setPixels(amplitudes, 0, resolution[0], 0, 0, resolution[0], resolution[1]);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                amplitudeView.setImageBitmap(Bitmap.createScaledBitmap(bmp,
                        resolution[0] * scaleFactor,
                        resolution[1] * scaleFactor, false));
            }
        });
    }

    public void openCamera() {
        Log.d(LOG_TAG, "openCamera");
        //check permission and request if not granted yet
        manager = (UsbManager) getSystemService(Context.USB_SERVICE);
        if (manager != null) {
            Log.d(LOG_TAG, "Manager valid");
        }

        HashMap<String, UsbDevice> deviceList = manager.getDeviceList();
        Log.d(LOG_TAG, "USB Devices : " + deviceList.size());

        Iterator<UsbDevice> iterator = deviceList.values().iterator();
        UsbDevice device;
        boolean found = false;
        while (iterator.hasNext()) {
            device = iterator.next();
            if (device.getVendorId() == 0x0451 || device.getProductId() == 0x9107) {
                Log.d(LOG_TAG, "Depth Camera found");
                found = true;
                if (!manager.hasPermission(device)) {
                    Intent intent = new Intent(ACTION_USB_PERMISSION);
                    intent.setAction(ACTION_USB_PERMISSION);
                    mUsbPi = PendingIntent.getBroadcast(this, 0, intent, 0);
                    manager.requestPermission(device, mUsbPi);
                } else {
                    performUsbPermissionCallback(device);
                    createBitmap();
                }
                break;
            }
        }
        if (!found) {
            Log.e(LOG_TAG, "No Depth Camera found!!!");
        }
    }

    public void closeCamera(){
        Log.d(LOG_TAG, "close camera");
        CloseCameraNative();
    }

    private void performUsbPermissionCallback(UsbDevice device) {
        usbConnection = manager.openDevice(device);
        Log.i(LOG_TAG, "permission granted for: " + device.getDeviceName() + ", fileDesc: " + usbConnection.getFileDescriptor());

        int fd = usbConnection.getFileDescriptor();
        DepthEyeCameraFd = fd;
        boolean ret = DepthEyeSystemNative(0x0451,0x9107,DepthEyeCameraFd, DepthEyeLibPath);
        Log.d(LOG_TAG, "performUsbPermissionCallback: ret = " + ret);
        SetModeNative(1);// mode 1 means :standard
        // EnableFilterHDRNative();
        // EnableFilterFlyingPixelNative(500);
        RegisterRawDataCallbackNative();

        boolean res = OpenCameraNative();;
        if (res) {
            m_opened = true;
            resolution = GetResolutionNative();
        }

    }

    private void createBitmap() {
        // calculate scale factor, which scales the bitmap relative to the disyplay resolution
        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        double displayWidth = size.x * 0.9;
        scaleFactor = (int) displayWidth / resolution[0];
        Log.d(LOG_TAG, "createBitmap res_w=[" + resolution[0] + "] res_h=[" + resolution[1] + "]");
        if (bmp == null) {
            bmp = Bitmap.createBitmap(resolution[0], resolution[1],
                    Bitmap.Config.ARGB_8888);
        }
    }
}