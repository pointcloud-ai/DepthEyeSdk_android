# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
MESSAGE("CMakeLists dir: ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE("cmake system name: ${CMAKE_SYSTEM_NAME}")
set(MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/main")
set(JNI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/main/jni")
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/main/jni/third_party")
set(CMAKE_VERBOSE_MAKEFILE true)
ADD_DEFINITIONS(-DAndroid)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -funwind-tables  ")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -funwind-tables")
set(ANDROID_ABI "armeabi-v7a")

set(VOXEL_INCLUDE "${THIRD_PARTY_DIR}/include/voxel-0.6.7/")
set(LIBUSB_INCLUDE "${THIRD_PARTY_DIR}/include/libusb/")


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(LIBUSB_DIR "${THIRD_PARTY_DIR}/lib/libusb/")
set(LIB_VOXEL_DIR "${THIRD_PARTY_DIR}/lib/")
set(LIB_TI3DTOF_DIR "${THIRD_PARTY_DIR}/lib/")

add_library(libvoxel SHARED IMPORTED)
set_target_properties(libvoxel PROPERTIES IMPORTED_LOCATION
                       ${LIB_VOXEL_DIR}/libvoxel.so)

add_library(libti3dtof SHARED IMPORTED)
set_target_properties(libti3dtof PROPERTIES IMPORTED_LOCATION
                       ${LIB_TI3DTOF_DIR}/libti3dtof.so)

add_library(libdeptheyeh1cdk SHARED IMPORTED)
set_target_properties(libdeptheyeh1cdk PROPERTIES IMPORTED_LOCATION
                       ${LIB_TI3DTOF_DIR}/voxel/libdeptheyeh1cdk.so)

add_library(usb-1.0 SHARED IMPORTED)
set_target_properties(usb-1.0 PROPERTIES IMPORTED_LOCATION
                      ${LIBUSB_DIR}/${ANDROID_ABI}/libusb1.0.so)


add_library(deptheye SHARED
            src/main/jni/ai_pointcloud_demo_depthcamera_MainActivity.cpp
            src/main/jni/DepthEyeInterface.cpp
            )
set_target_properties(deptheye PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY ${MAIN_DIR}/jniLibs/${ANDROID_ABI})


find_library( # Sets the name of the path variable.
               # Specifies the name of the NDK library that
               # you want CMake to locate.
               log-lib
               log )
set(COMMON_INCLUDE ${VOXEL_INCLUDE} ${LIBUSB_INCLUDE})

set(COMMON_LIBS libvoxel libti3dtof libdeptheyeh1cdk usb-1.0)

include_directories(${COMMON_INCLUDE})

target_link_libraries(deptheye ${COMMON_LIBS} android log)