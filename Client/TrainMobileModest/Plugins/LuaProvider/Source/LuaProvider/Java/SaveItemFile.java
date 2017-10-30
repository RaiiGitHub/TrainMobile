package com.DeepEye.TrainMobileModest;

import java.io.IOException;
import java.io.RandomAccessFile;
/**
 * <b>function:</b> 写入文件、保存文件
 * @author hoojo
 * @createDate 2011-9-21 下午05:44:02
 * @file SaveItemFile.java
 * @package com.hoo.download
 * @project MultiThreadDownLoad
 * @blog http://blog.csdn.net/IBM_hoojo
 * @email hoojo_@126.com
 * @version 1.0
 */

public class SaveItemFile {
    //存储文件
    private RandomAccessFile itemFile;
    public SaveItemFile() throws IOException {
        this("", 0);
    }
    /**
     * @param name 文件路径、名称
     * @param pos 写入点位置 position
     * @throws IOException
     */
    public SaveItemFile(String name, long pos) throws IOException {
        itemFile = new RandomAccessFile(name, "rwd");
        //在指定的pos位置开始写入数据
        itemFile.seek(pos);
    }
    /**
     * <b>function:</b> 同步方法写入文件,don't need to. for improving writting speed.
     * @author hoojo
     * @createDate 2011-9-26 下午12:21:22
     * @param buff 缓冲数组
     * @param start 起始位置
     * @param length 长度
     * @return
     */

    public /*synchronized*/ int write(byte[] buff, int start, int length) {
        int i = 0;
        try {
            itemFile.write(buff, start, length);
            i = length;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return i;
    }

    public void close() throws IOException {
        if (itemFile != null) {
            itemFile.close();
        }
    }
}