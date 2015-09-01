//
// Created by  XinghuiQuan on 8/5/15.
//

#include <jni.h>

#ifndef NATIVEDAEMONPROCESS_DAEMON_H
#define NATIVEDAEMONPROCESS_DAEMON_H

#endif //NATIVEDAEMONPROCESS_DAEMON_H

extern "C" {
JNIEXPORT jint JNICALL Java_com_xinghui_nativedaemonprocess_DaemonNativeUtils_start(JNIEnv *,
                                                                                    jclass, jstring,
                                                                                    jstring);
};


extern "C" {
JNIEXPORT jint JNICALL
Java_com_xinghui_nativedaemonprocess_DaemonNativeUtils_connectToDaemonSocketServer(JNIEnv *env,
                                                                                   jclass type);

}
