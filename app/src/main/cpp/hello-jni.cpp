/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <jni.h>
#include <string>
#include <cassert>
#include <unistd.h>
#include <thread>         // std::thread

#define ABI "unknown"

// format function from here - https://github.com/cocos2d/cocos2d-x/blob/cocos2d-x-3.17/cocos/base/ccUTF8.cpp#L43
/*--- This a C++ universal sprintf in the future.
**  @pitfall: The behavior of vsnprintf between VS2013 and VS2015/2017 is different
**      VS2013 or Unix-Like System will return -1 when buffer not enough, but VS2015/2017 will return the actural needed length for buffer at this station
**      The _vsnprintf behavior is compatible API which always return -1 when buffer isn't enough at VS2013/2015/2017
**      Yes, The vsnprintf is more efficient implemented by MSVC 19.0 or later, AND it's also standard-compliant, see reference: http://www.cplusplus.com/reference/cstdio/vsnprintf/
*/
std::string format(const char* format, ...)
{
#define CC_VSNPRINTF_BUFFER_LENGTH 512
    va_list args;
    std::string buffer(CC_VSNPRINTF_BUFFER_LENGTH, '\0');

    va_start(args, format);
    unsigned long buffleng = buffer.length();
    int nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
    va_end(args);

    if (nret >= 0) {
        if ((unsigned int)nret < buffer.length()) {
            buffer.resize(nret);
        }
        else if ((unsigned int)nret > buffer.length()) { // VS2015/2017 or later Visual Studio Version
            buffer.resize(nret);

            va_start(args, format);
            nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
            va_end(args);

            assert(nret == buffer.length());
        }
        // else equals, do nothing.
    }
    else { // less or equal VS2013 and Unix System glibc implement.
        do {
            buffer.resize(buffer.length() * 3 / 2);
            buffleng = buffer.length();

            va_start(args, format);
            nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
            va_end(args);

        } while (nret < 0);

        buffer.resize(nret);
    }

    return buffer;
}

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   hello-jni/app/src/main/java/com/example/hellojni/HelloJni.java
 */
extern "C"
{
    JNIEXPORT jstring JNICALL
    Java_com_example_hellojni_HelloJni_stringFromJNI(JNIEnv *env,
                                                 jobject thiz) {

        std::thread _thread = std::thread([](){
            std::string test1 = format("Any text here. Language doesn't matter %d", 1);
            // sleep is important. vsnprint stop working (returns -1) after some delay, not immediately
            sleep(2);
            // non-english language is important
            std::string test2 = format("运送%d个乘客！电工、防火员和警察不算", 1);
        });
        _thread.detach();

        std::string str = "运送%d个乘客！电工、防火员和警察不算";
        std::string formatedText = format(str.c_str(), 4716);
    return (*env).NewStringUTF(formatedText.c_str());
    }
}

