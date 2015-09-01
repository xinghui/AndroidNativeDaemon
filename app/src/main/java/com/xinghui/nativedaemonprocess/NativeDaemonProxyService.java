package com.xinghui.nativedaemonprocess;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.os.Process;
import android.os.UserManager;
import android.support.annotation.Nullable;
import android.util.Log;

/**
 * Created by xinghui on 8/5/15.
 */
public class NativeDaemonProxyService extends Service {

    private static final String TAG = "NativeDaemonProxyService";

    @Override
    public void onCreate() {
        super.onCreate();
        Log.v(TAG, "NativeDaemonProxyService onCreate()");
        String str = DaemonNativeUtils.stringFromJNI();
        Log.i(TAG, "stringFromJNI = " + str);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Log.v(TAG, "NativeDaemonProxyService onStartCommand()");
        String userSerial = getUserSerial(this);
        int pid = DaemonNativeUtils.start(CommonDefine.PACKAGE_NAME, userSerial);
        Log.i(TAG, "DaemonNativeUtils.start pid = " + pid + ", userSerial = " + userSerial);

        // TODO should kill child progress??
        if (pid != 0) {// Only Kill father progress

            Intent intent1 = new Intent(DaemonManager.ACTION_DAEMON_START);
            sendBroadcast(intent1);

            int myPid = Process.myPid();
            int tid = Process.myTid();
            int uid = Process.myUid();
            Log.v(TAG, "myPid father = " + myPid + ", tid = " + tid + ", uid = " + uid);
            if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
                stopSelf();// Android 5.1 ANR
            }
            android.os.Process.killProcess(myPid);
        } else {
            // child progress
            int myPid = Process.myPid();
            int tid = Process.myTid();
            int uid = Process.myUid();
            Log.v(TAG, "myPid  child = " + myPid + ", tid = " + tid + ", uid = " + uid);
        }

        return super.onStartCommand(intent, flags, startId);
    }

    public static String getUserSerial(Context context) {
        UserManager userManager = (UserManager) context.getSystemService(Context.USER_SERVICE);
        if (userManager != null) {
            long serial = userManager.getSerialNumberForUser(android.os.Process.myUserHandle());
            Log.v(TAG, "serial = " + serial);
            return String.valueOf(serial);
        }
        return "";
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        int myPid = Process.myPid();
        int tid = Process.myTid();
        int uid = Process.myUid();
        Log.v(TAG, "myPid onDestroy = " + myPid + ", tid = " + tid + ", uid = " + uid);
        super.onDestroy();
    }

    static {
        // This is a new process, should load again;
        System.loadLibrary("daemon");
    }
}
