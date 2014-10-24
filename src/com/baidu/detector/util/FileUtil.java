package com.baidu.detector.util;

import java.io.File;

import android.os.Environment;

public class FileUtil {
	public static String getSDCardPath()
	{
		File sdDir = null;
		boolean sdCardExist = Environment.getExternalStorageState()
		           .equals(android.os.Environment.MEDIA_MOUNTED); //�ж�sd���Ƿ����
		if (sdCardExist)
		{
			sdDir = Environment.getExternalStorageDirectory();//��ȡ��Ŀ¼
		}
		else {
			return null;
		}
		return sdDir.toString();
	}
}
