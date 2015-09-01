package com.xinghui.nativedaemonprocess;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.*;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintWriter;

/**
 * Created by xinghui on 8/5/15.
 */
public class DaemonManager {

    private static final String TAG = "DaemonManager";

    public static final String ACTION_DAEMON_START = "com.xinghui.ACTION_DAEMON_START";


    private static DaemonManager mDaemonManager;

    public static DaemonManager getInstance() {
        if (mDaemonManager == null) {
            synchronized (DaemonManager.class) {
                if (mDaemonManager == null) {
                    mDaemonManager = new DaemonManager();
                }
            }
        }
        return mDaemonManager;
    }

    public static void tryConnectToDaemonProcess(Context context) {
        Log.v(TAG, "xinghui recv RestartService onStartCommand() ");
        int result = DaemonNativeUtils.connectToDaemonSocketServer();
        Log.v(TAG, "xinghui recv RestartService onStartCommand() result = " + result);
        if (result != 0) {
            DaemonManager.startNativeDaemonProxyService(context);
        }

    }

    public static void startNativeDaemonProxyService(Context context) {
        Intent intent = new Intent(context, NativeDaemonProxyService.class);
        intent.setPackage(CommonDefine.PACKAGE_NAME);
        context.startService(intent);
    }

    private DaemonBroadcastReceiver daemonBroadcastReceiver = new DaemonBroadcastReceiver();

    public void registerBroadcast(Context context) {
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_DAEMON_START);
        context.registerReceiver(daemonBroadcastReceiver, filter);
    }

    public class DaemonBroadcastReceiver extends BroadcastReceiver {

        private static final String SERVER_NAME = "xinghui_server";

        private boolean mConnected = false;

        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent == null) return;

            if (ACTION_DAEMON_START.equals(intent.getAction())) {

                if (mConnected) return;

                Log.i(TAG, "ACTION_DAEMON_START mConnected = " + mConnected);
                int result = DaemonNativeUtils.connectToDaemonSocketServer();
                Log.i(TAG, "ACTION_DAEMON_START result = " + result);
                mConnected = (result == 0);

                if (!mConnected) {
                    startNativeDaemonProxyService(context);
                } else {
                    context.unregisterReceiver(this);
                }
            }
        }

    }
}
