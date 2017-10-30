package com.DeepEye.TrainMobileModest;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.io.RandomAccessFile;
import com.DeepEye.TrainMobileModest.DownloadInfo;
import com.DeepEye.TrainMobileModest.LogUtils;
import com.DeepEye.TrainMobileModest.ApkDownloader;


/**
 * <b>function:</b> 分批量下载文件
 * @author hoojo
 * @createDate 2011-9-22 下午05:51:54
 * @file BatchDownloadFile.java
 * @project MultiThreadDownLoad
 * @blog http://blog.csdn.net/IBM_hoojo
 * @email hoojo_@126.com
 * @version 1.0
 */

public class BatchDownloadFile implements Runnable {
    //下载文件信息 
    private DownloadInfo downloadInfo;
    //一组开始下载位置
    private long[] startPos;
    //一组结束下载位置
    private long[] endPos;
    //休眠时间
    private static final int SLEEP_SECONDS = 500;
    //子线程下载
    private DownloadFile[] fileItem;
    //文件长度
    private int length;
    //是否第一个文件
    private boolean first = true;
    //是否停止下载
    private boolean stop = false;
    //临时文件信息
    private File tempFile;
    public BatchDownloadFile(DownloadInfo downloadInfo) {
        this.downloadInfo = downloadInfo;
        String tempPath = this.downloadInfo.getFilePath() + File.separator + this.downloadInfo.getFileName() + ".position";
        tempFile = new File(tempPath);

        if( tempFile.exists() )
            tempFile.delete();//delete right now!
        File targetFile = new File(this.downloadInfo.getFilePath() + File.separator + this.downloadInfo.getFileName());
        if( targetFile.exists() )
            targetFile.delete();//delete right now!
        
        //如果存在读入点位置的文件
        if (tempFile.exists()) {
            first = false;
            //就直接读取内容
            try {
                readPosInfo();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {
            //数组的长度就要分成多少段的数量
            startPos = new long[downloadInfo.getSplitter()];
            endPos = new long[downloadInfo.getSplitter()];
        }
    }

    @Override
    public void run() {
        //首次下载，获取下载文件长度
        if (first) {
            length = this.getFileSize();//获取文件长度
            ApkDownloader.total_len = length;
            ApkDownloader.cur_len = 0;
            if (length == -1) {
                String err = "file length isn't known!";
                LogUtils.log(err);
                stop = true;
                ApkDownloader.makeWidgetText(err);
            } else if (length == -2) {
                String err = "file length being read has occured an error!";
                LogUtils.log(err);
                ApkDownloader.makeWidgetText(err);
                stop = true;
            } else if (length > 0) {
                /**
                 * eg 
                 * start: 1, 3, 5, 7, 9
                 * end: 3, 5, 7, 9, length
                 */
                try {
                     //创建一个random access文件
                    RandomAccessFile raf  = new RandomAccessFile( this.downloadInfo.getFilePath() + File.separator + this.downloadInfo.getFileName(), "rw");
                    raf.setLength(length);
                    raf.close();
                    ApkDownloader.makeWidgetText("开始下载，下载完成后请按提示安装更新...");
                    for (int i = 0, len = startPos.length; i < len; i++) {
                        int size = i * (length / len);
                        startPos[i] = size;
                        //设置最后一个结束点的位置
                        if (i == len - 1) {
                            endPos[i] = length;
                        } else {
                            size = (i + 1) * (length / len);
                            endPos[i] = size;
                        }
                        LogUtils.log("start-end Position[" + i + "]: " + startPos[i] + "-" + endPos[i]);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            } else {
                String err = "getting file length has been occured an error, download has been stopped!";
                LogUtils.log(err);
                ApkDownloader.makeWidgetText(err);
                stop = true;
            }
        }

        //子线程开始下载
        if (!stop) {
            //创建单线程下载对象数组
            fileItem = new DownloadFile[startPos.length];//startPos.length = downloadInfo.getSplitter()
            for (int i = 0; i < startPos.length; i++) {
                try {
                    //创建指定个数单线程下载对象，每个线程独立完成指定块内容的下载
                    fileItem[i] = new DownloadFile(
                        downloadInfo.getUrl(), 
                        this.downloadInfo.getFilePath() + File.separator + this.downloadInfo.getFileName(), 
                        startPos[i], endPos[i], i
                    );
                    fileItem[i].start();//启动线程，开始下载
                    LogUtils.log("Thread: " + i + ", startPos: " + startPos[i] + ", endPos: " + endPos[i]);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            //循环写入下载文件长度信息
            while (!stop) {
                try {
                    writePosInfo();
                    LogUtils.log("downloading……");
                    Thread.sleep(SLEEP_SECONDS);
                    stop = true;
                } catch (IOException e) {
                    e.printStackTrace();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                for (int i = 0; i < startPos.length; i++) {
                    if (!fileItem[i].isDownloadOver()) {
                        stop = false;
                        break;
                    }
                }
            }
            LogUtils.log("Download task is finished!");
            this.tempFile.delete();
            ApkDownloader.notifyDownloadFinished(this.downloadInfo.getFilePath() + File.separator + this.downloadInfo.getFileName());
        }
    }
    /**
     * 将写入点数据保存在临时文件中
     * @author hoojo
     * @createDate 2011-9-23 下午05:25:37
     * @throws IOException
     */
    private void writePosInfo() throws IOException {
        DataOutputStream dos = new DataOutputStream(new FileOutputStream(tempFile));
        dos.writeInt(startPos.length);
        for (int i = 0; i < startPos.length; i++) {
            dos.writeLong(fileItem[i].getStartPos());
            dos.writeLong(fileItem[i].getEndPos());
            LogUtils.log("[" + fileItem[i].getStartPos() + "#" + fileItem[i].getEndPos() + "]");
        }
        dos.close();
    }
    /**
     * <b>function:</b>读取写入点的位置信息
     * @author hoojo
     * @createDate 2011-9-23 下午05:30:29
     * @throws IOException
     */
    private void readPosInfo() throws IOException {
        DataInputStream dis = new DataInputStream(new FileInputStream(tempFile));
        int startPosLength = dis.readInt();
        startPos = new long[startPosLength];
        endPos = new long[startPosLength];
        for (int i = 0; i < startPosLength; i++) {
            startPos[i] = dis.readLong();
            endPos[i] = dis.readLong();
        }
        dis.close();
    }

    /**
     * <b>function:</b> 获取下载文件的长度
     * @author hoojo
     * @createDate 2011-9-26 下午12:15:08
     * @return
     */
    private int getFileSize() {
        int fileLength = -1;
        try {
            URL url = new URL(this.downloadInfo.getUrl());
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            DownloadFile.setHeader(conn);
            int stateCode = conn.getResponseCode();
            //判断http status是否为HTTP/1.1 206 Partial Content或者200 OK
            if (stateCode != HttpURLConnection.HTTP_OK && stateCode != HttpURLConnection.HTTP_PARTIAL) {
                LogUtils.log("Error Code: " + stateCode);
                return -2;
            } else if (stateCode >= 400) {
                LogUtils.log("Error Code: " + stateCode);
                return -2;
            } else {
                //获取长度
                fileLength = conn.getContentLength();
                LogUtils.log("FileLength: " + fileLength);
            }
            DownloadFile.printHeader(conn);
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return fileLength;
    }
}