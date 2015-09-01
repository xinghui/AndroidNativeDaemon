package com.xinghui.nativedaemonprocess;

/**
 * Created by xinghui on 8/5/15.
 */
public class DaemonNativeUtils {

    public static native String stringFromJNI();

    public native static int start(String packageName, String user);

    public native static int connectToDaemonSocketServer();
}
