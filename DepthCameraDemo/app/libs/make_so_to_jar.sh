#make jar
rm armeabi-v7a.jar
rm -r ../src/main/assets/lib/*
mkdir -p ./lib/armeabi-v7a
cp ../src/main/jniLibs/armeabi-v7a/libvoxel.so ../src/main/jniLibs/armeabi-v7a/libvoxel.so.0.6.7
cp ../src/main/jniLibs/armeabi-v7a/libvoxel.so ../src/main/jniLibs/armeabi-v7a/libvoxel.so.34
cp ../src/main/jniLibs/armeabi-v7a/libti3dtof.so ../src/main/jniLibs/armeabi-v7a/libti3dtof.so.0.6.7
cp ../src/main/jniLibs/armeabi-v7a/libti3dtof.so ../src/main/jniLibs/armeabi-v7a/libti3dtof.so.34
cp ../src/main/jniLibs/armeabi-v7a/libdeptheyeh1cdk.so ../src/main/jniLibs/armeabi-v7a/libdeptheyeh1cdk.so.0.6.7
cp ../src/main/jniLibs/armeabi-v7a/libdeptheyeh1cdk.so ../src/main/jniLibs/armeabi-v7a/libdeptheyeh1cdk.so.34
cp ../src/main/jniLibs/armeabi-v7a/lib* ./lib/armeabi-v7a/
cp ../src/main/jniLibs/armeabi-v7a/lib* ../src/main/assets/lib/
cp /Users/jiandonghuang/Documents/Personal/Projects/VoxelSDK_Mac_V2/TI3DToF/boards/DepthEyeH1CDK/*.conf ./lib/armeabi-v7a/
cp /Users/jiandonghuang/Documents/Personal/Projects/VoxelSDK_Mac_V2/TI3DToF/boards/DepthEyeH1CDK/*.bin ./lib/armeabi-v7a/
cp /Users/jiandonghuang/Documents/Personal/Projects/VoxelSDK_Mac_V2/TI3DToF/OPT8320.dml ./lib/armeabi-v7a/
zip -q -r armeabi-v7a2.zip lib
mv armeabi-v7a2.zip armeabi-v7a.jar
rm -r ./lib
