//
// Created by  XinghuiQuan on 8/5/15.
//

#include <jni.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/signal.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "Common.h"


#define BUFFSIZE 32
#define FP fprintf

#include <android/log.h>
//#define TAG "daemon.c"
#define TAG "xinghui"

// some phone can't print INFO Log
#define LOGI(...) __android_log_print(ANDROID_LOG_ERROR,TAG, "--------INFO--" __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG, "--------ERROR---" __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, "--------ERROR---" __VA_ARGS__)


int main(int argc) {

}

typedef struct _JNI_THREAD_OBJ {
//    int connectfd;
    int socketfd;
    const char *user;
    const char *packageName;
} JNI_THREAD_OBJ;


void *fun_in_thread(void *args) {

    JNI_THREAD_OBJ *jniThreadObj = (JNI_THREAD_OBJ *) args;

//    int connectfd = jniThreadObj->connectfd;;
    const char *user = jniThreadObj->user;
    const char *packageName = jniThreadObj->packageName;

    LOGI("fun_in_thread()11 packageName = %s, user = %s", packageName, user);
//    LOGI("fun_in_thread()22 socketfd = %d, packageName = %s, user = %s", jniThreadObj->socketfd, jniThreadObj->packageName, jniThreadObj->user);


    int connectfd;
    int socketfd;
    sockaddr_un addr;

    // Local Communication
    if ((socketfd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) {
        return 0;
    }

//    unlink(filePath);// method 1
    addr.sun_family = PF_LOCAL;
    strcpy(addr.sun_path, SERVER_NAME);// method 1

    unlink(SERVER_NAME);

//    memset(&addr, 0, sizeof(addr));



//    addr.sun_path[0] = 0;// method 2
//    strcpy(addr.sun_path + 1, SERVER_NAME);// method 2


//    int server_addr_length = sizeof(addr); // method 1
//    int server_addr_length = strlen(SERVER_NAME) + offsetof(struct sockaddr_un, sun_path);// method 2
    int server_addr_length = sizeof(addr.sun_family) + sizeof(SERVER_NAME);

    LOGI("sun_path is %s, server_addr_length = %d, sun_path = %s", SERVER_NAME, server_addr_length,
         &addr.sun_path);

    LOGI("bind start");
    if (bind(socketfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
//    if(bind(socketfd, (struct sockaddr*) &addr, server_addr_length) < 0)
    {
        LOGE("bind socket error : %d", errno);
        unlink(SERVER_NAME);
        return 0;
    }
    LOGI("bind ok");

    LOGI("listen start");
    if (listen(socketfd, 10) == -1) {
        LOGI("listen socket error : %d", errno);
        unlink(SERVER_NAME);
        return 0;
    }
    LOGI("listen ok");


    sockaddr_un client_addr;
    socklen_t client_addr_len;
    client_addr_len = sizeof(client_addr);
    while (1) {
        LOGE("while!!!!!!!!");
        connectfd = accept(socketfd, (struct sockaddr *) NULL, NULL);
        LOGE("while!!!!!!!! %d", connectfd);

        if (connectfd < 0) {
            LOGE("accept!!!!!!!!");
            if (errno == EINTR) {
                LOGE("accept error == EINTR  (EINTR = %d)", EINTR);
//                continue;
                return 0;
            }
            else {
                LOGI("accept error : %d", errno);
                return 0;
            }
        }
        else {
            LOGI("sss");
            char buff[1024];
            int n = recv(connectfd, buff, sizeof(buff), 0);
            LOGI("xinghui recv n = %d", n);
            if (n < 0) {
                LOGI("xinghui recv n < 0, n = %d", n);
                LOGI("ead failed (%s), n = %d ", strerror(errno), n);
            }
            else if (n == 0) {

                int user_len = strlen(user);
                LOGI("1channel detached remotely, user_len = %d", user_len);
                close(connectfd);
                char c[128];
                if (user_len >= 0) {
//                    sprintf(c, "am startservice --user 0 -n %s/.daemon.RestartService", packageName);
                    sprintf(c,
                            "am startservice --user 0 -n com.xinghui.nativedaemonprocess/.daemon.RestartService");
                } else {
                    sprintf(c, "am startservice -n %s/.daemon.RestartService", packageName);
                }
                LOGI("str length: %d, %s", strlen(c), c);
                LOGI("xinghui recv n == 0");
                system(c);
//                return 0;//break;
            }
            char *cp = buff;
            LOGI("cp----- %s", cp);
//            kill(getpid(), SIGQUIT);
        }
    }
}

int create_socket(/*const char* filePath, */const char *packageName, const char *user) {


    pthread_t pt;
    JNI_THREAD_OBJ *jniThreadObj = new JNI_THREAD_OBJ;
//    jniThreadObj->connectfd = connectfd;
//    jniThreadObj->socketfd = socketfd;
    jniThreadObj->packageName = packageName;
    jniThreadObj->user = user;
//    LOGI("create_socket()11  socketfd = %d, packageName = %s, user = %s", socketfd, packageName, user);
//    LOGI("create_socket()22  socketfd = %d, packageName = %s, user = %s", jniThreadObj->socketfd, jniThreadObj->packageName, jniThreadObj->user);
    pthread_create(&pt, NULL, fun_in_thread, jniThreadObj);
    return 0;

}

extern "C" {

JNIEXPORT jint JNICALL Java_com_xinghui_nativedaemonprocess_DaemonNativeUtils_start(JNIEnv *env,
                                                                                    jclass cls,
                                                                                    jstring packageName,
                                                                                    jstring user) {
    const char *packageName_char = env->GetStringUTFChars(packageName, NULL);
    env->ReleaseStringUTFChars(packageName, packageName_char);

//    const char* path_char = env->GetStringUTFChars(path, NULL);
//    env->ReleaseStringUTFChars(path, path_char);

    const char *user_char = NULL;
    if (user != NULL) {
        user_char = env->GetStringUTFChars(user, NULL);
        env->ReleaseStringUTFChars(user, user_char);
    }

    pid_t pid;
    pid_t sid;

    LOGI("fork");
    pid = fork();

    if (pid == 0) {
        LOGI("daemon process");
        const char *new_name;

        new_name = "xinghui";// TODO for test
        prctl(PR_SET_NAME, new_name, 0, 0, 0);
//        LOGI("daemon process group id = %d", getpgrp());
        sid = setsid();
//        LOGI("daemon process group id = %d, sid = %d, pid = %d", getpgrp(), sid, getpid());
        if (sid < 0) {
            LOGI("setsid sid = %d ", sid);
        }

//        signal(SIGINT, SIG_IGN);
//        signal(SIGHUP, SIG_IGN);
//        signal(SIGQUIT, SIG_IGN);
//        signal(SIGPIPE, SIG_IGN);
//        signal(SIGTTOU, SIG_IGN);
//        signal(SIGTTIN, SIG_IGN);
//        signal(SIGCHLD, SIG_IGN);
//        signal(SIGTERM, SIG_IGN);
//
//        signal(SIGSEGV, SIG_IGN);

//        struct sigaction sig;
//        sig.sa_handler = SIG_IGN;
//        sig.sa_flags = 0;
//        sigemptyset(&sig.sa_mask);
//        sigaction(SIGPIPE, &sig, NULL);// TODO check

        chdir("/");
        umask(0);
        create_socket(packageName_char, user_char);
    }
    else if (pid > 0) {
        LOGI("main process pid = %d", pid);
    }
    return pid;

}
}

extern "C" {

jstring Java_com_xinghui_nativedaemonprocess_DaemonNativeUtils_stringFromJNI(JNIEnv *env,
                                                                             jclass thiz) {
#if defined(__arm__)
    #if defined(__ARM_ARCH_7A__)
      #if defined(__ARM_NEON__)
        #if defined(__ARM_PCS_VFP)
          #define ABI "armeabi-v7a/NEON (hard-float)"
        #else
          #define ABI "armeabi-v7a/NEON"
        #endif
      #else
        #if defined(__ARM_PCS_VFP)
          #define ABI "armeabi-v7a (hard-float)"
        #else
          #define ABI "armeabi-v7a"
        #endif
      #endif
    #else
     #define ABI "armeabi"
    #endif
#elif defined(__i386__)
    #define ABI "x86"
#elif defined(__x86_64__)
    #define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
    #define ABI "mips64"
#elif defined(__mips__)
    #define ABI "mips"
#elif defined(__aarch64__)
#define ABI "arm64-v8a"
#else
    #define ABI "unknown"
#endif
    LOGV("------");
    jstring str = env->NewStringUTF("Hello from JNI! Compiled with ABI " ABI ".");
    return str;
}
}

extern "C" {
JNIEXPORT jint JNICALL Java_com_xinghui_nativedaemonprocess_DaemonNativeUtils_connectToDaemonSocketServer(
        JNIEnv *env, jclass cls) {


    struct sockaddr_un addr;
    int retry_time = 5;
    int result = -1;

    size_t namelen = strlen(SERVER_NAME);

    while (retry_time) {
        retry_time--;

        sleep(2);


//        memset(&addr, 0, sizeof(addr));
//        addr.sun_family = PF_LOCAL;
//        addr.sun_path[0] = 0;
//        memcpy(addr.sun_path + 1, SERVER_NAME, namelen);
//
//        socklen_t alen = namelen + offsetof(struct sockaddr_un, sun_path) + 1;
////        strcpy(addr.sun_path + 1, SERVER_NAME);


        int socketfd = socket(PF_LOCAL, SOCK_STREAM, 0);
        if (socketfd == -1) {
            LOGV("main process create socket error: %d", errno);
            continue;
        }

        addr.sun_family = PF_LOCAL;
        strcpy(addr.sun_path, SERVER_NAME);

//        int server_addr_length = strlen(SERVER_NAME) + offsetof(struct sockaddr_un, sun_path);
//        int server_addr_length = sizeof(addr.sun_family) + sizeof(SERVER_NAME);
        if (connect(socketfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            LOGV("connect socket failed retry_time = %d, errorno = %d", retry_time, errno);
            close(socketfd);
            continue;
        }

        else {
            result = 0;
            LOGV("connect socket success_____________ retry_time = %d", retry_time);
            return result;
        }
    }
    return result;
}
}