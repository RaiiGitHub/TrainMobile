package com.DeepEye.TrainMobileModest;
/**
 * <b>function:</b> 日志工具类
 * @author hoojo
 * @createDate 2011-9-21 下午05:21:27
 * @file LogUtils.java
 * @project MultiThreadDownLoad
 * @blog http://blog.csdn.net/IBM_hoojo
 * @email hoojo_@126.com
 * @version 1.0
 */

public abstract class LogUtils {
    public static void err(Object message) {
        System.err.println("LogUtils::"+message);
    }

    public static void err(String message) {
        System.err.println("LogUtils::"+message);
    }

    public static void err(int message) {
        System.err.println("LogUtils::"+message);
    }

    public static void log(Object message) {
        System.out.println("LogUtils::"+message);
    }

    public static void log(String message) {
        System.out.println("LogUtils::"+message);
    }
    
    public static void log(int message) {
        System.out.println("LogUtils::"+message);
    }
}