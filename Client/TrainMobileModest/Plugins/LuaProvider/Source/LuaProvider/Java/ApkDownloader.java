package com.DeepEye.TrainMobileModest;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.content.IntentSender.SendIntentException;

import java.io.File;
import android.net.Uri;
import android.app.Activity;
import android.os.Message;
import android.os.Handler;
import android.os.Bundle;
import com.DeepEye.TrainMobileModest.DownloadUtils;
import com.epicgames.ue4.GameActivity;


/**
 * <b>function:</b> 下载类
 * @author Raii
 * @createDate 2017.5.20
 * @version 1.0
 */
public class ApkDownloader {
    public static GameActivity gameActivity;
    public static int total_len;
    public static int cur_len;
    public static void notifyDownloadProcess(int downloaded)
    {
        cur_len += downloaded;
        gameActivity.nativeDownloadPercent(1, total_len, cur_len);//it was treated as one thread.
    }
    public static void notifyDownloadFinished(String fileName)
    {
        Message message = Message.obtain();
        Bundle bundle = new Bundle();
		message.what = 2;
        bundle.putString("apkfile", fileName);
		message.setData(bundle);
		gameActivity.handler.sendMessage(message);
    }
    public static void makeWidgetText(String what)
    {
        Message message = Message.obtain();
        Bundle bundle = new Bundle();
		message.what = 1;
        bundle.putString("msg", what);
		message.setData(bundle);
		gameActivity.handler.sendMessage(message);
    }

    public static void begin(String apkURL,GameActivity activity) {
        gameActivity = activity;
        total_len = 0;
        cur_len = 0;
        DownloadUtils.download(apkURL, "", "", 5);
    }
}

