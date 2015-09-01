package com.xinghui.nativedaemonprocess;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import java.io.File;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        DaemonManager.getInstance().registerBroadcast(this);

        new Thread(new Runnable() {
            @Override
            public void run() {
                String fileDir = MainActivity.this.getFilesDir().toString();
                String daemonSocketFile = fileDir + File.separator + "daemon";
                File daemonFile = new File(daemonSocketFile);
                if (!daemonFile.exists()) {
                    try {
                        daemonFile.createNewFile();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }

                Log.i(TAG, "fileDir = " + fileDir + ", isFile = " + daemonFile.isFile() + ", daemonSocketFile = " + daemonSocketFile);
                DaemonManager.tryConnectToDaemonProcess(MainActivity.this);
            }
        }).start();
    }

    static {
        // This is a new process, should load again;
        System.loadLibrary("daemon");
    }
}
