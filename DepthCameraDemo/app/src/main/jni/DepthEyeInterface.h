//
// Created by Jiandong Huang on 3/2/2018.
//

#ifndef DEPTHCAMERADEMO_DEPTHEYEINTERFACE_H
#define DEPTHCAMERADEMO_DEPTHEYEINTERFACE_H

#include <dirent.h>
#include "CameraSystem.h"
#include "Common.h"
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO , "ProjectName", __VA_ARGS__)
namespace PointCloud
{
	enum DEPTH_MODE{
		UNKNOWN_MODE = 0,
		STANDARD,
		PRICISTION
	};

	enum DEVICE_STATUS
	{
		UNKNOWN_STATUS = 0,
		INITIALIZED,
		CONNECTED,
		STARTED,
		STOP
	};

	struct FrameSize
	{
	  uint32_t width, height;
	};

	class DepthEyeSystem
	{
	public:
		DepthEyeSystem();
		DepthEyeSystem(int vid,int pid, int usbFd, const char* libPath);
		void setMode(DEPTH_MODE mode);
		DEVICE_STATUS connect();
		bool isInitialiszed();
		bool enableFilterHDR();
		bool enableFilterFlyingPixel(int threshold);
		void registerPointCloudCallback(Voxel::DepthCamera::CallbackType f);
		void registerRawDataCallback(Voxel::DepthCamera::CallbackType f);
		bool start();
		bool stop();
		void reset();
		bool disconnect();
		float  getFrameRate();
		float  getFOV();
		FrameSize  getRevolution();
	private:
		Voxel::CameraSystem*	  cameraSys_;
		Voxel::DepthCameraPtr depthCamera_;
		Voxel::DevicePtr 	  device_;
		DEPTH_MODE 			  mode_;
		DEVICE_STATUS 		  status_;

		void setEnv(const char* libraryPath);
		void processDIR(const char *dir,std::vector<std::string> &vec,int type,const char *namestr);

		Voxel::DepthCamera::CallbackType pontcloud_func_;
		Voxel::DepthCamera::CallbackType rawdata_func_;
	};
}

#endif //DEPTHCAMERADEMO_DEPTHEYEINTERFACE_H
