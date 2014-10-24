package com.baidu.trafficsignrecon;

import org.opencv.core.Mat;

public class TrafficsignRecon {
    public TrafficsignRecon() {
    	// �ļ�·��д���ں�����
        mNativeObj = nativeCreateObject();
    }

    public long detect(Mat imageGray) {
        return nativeDetect(mNativeObj, imageGray.getNativeObjAddr());
    }

    public void release() {
        nativeDestroyObject(mNativeObj);
        mNativeObj = 0;
    }

    private long mNativeObj = 0;

    private static native long nativeCreateObject();
    private static native void nativeDestroyObject(long thiz);
    private static native long nativeDetect(long thiz, long inputImage);
}
