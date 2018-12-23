cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_ANDROID_STL_TYPE "c++_static")

add_library(hello-jni SHARED
            hello-jni.cpp)
include(AndroidNdkModules)
android_ndk_import_module_cpufeatures()
find_library(log-lib log)
# Include libraries needed for hello-jni lib
target_link_libraries(hello-jni
                      ${log-lib})
