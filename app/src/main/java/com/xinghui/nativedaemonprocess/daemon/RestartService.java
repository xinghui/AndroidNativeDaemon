package com.xinghui.nativedaemonprocess.daemon;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.util.Log;

import com.xinghui.nativedaemonprocess.DaemonManager;
import com.xinghui.nativedaemonprocess.DaemonNativeUtils;

/**
 * Created by xinghui on 8/6/15.
 */
public class RestartService extends Service{

    private static final String TAG = "RestartService";

    @Override
    public void onCreate() {
        super.onCreate();
        Log.v(TAG, "RestartService onCreate()");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        DaemonManager.tryConnectToDaemonProcess(this);
        return super.onStartCommand(intent, flags, startId);
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    static {
        System.loadLibrary("daemon");
    }
}
