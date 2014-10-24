/*
 * TrafficplateRecon_jni.cpp
 *
 *  Created on: 2014-8-6
 *      Author: houwenbo
 */

#include "TrafficplateRecon_jni.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/detection_based_tracker.hpp>

#include <string>
#include <vector>

#include "lisenceplate/eblearn/src/carLicensePlate.h"

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <time.h>
#include "lisenceplate/eblearn/src/libeblearn.h"
#include "lisenceplate/eblearn/src/libeblearntools.h"
#include "lisenceplate/eblearn/src/eblapp.h"

#include "file_util.h"

#include <android/log.h>


#include "util.h"

using namespace std;
using namespace cv;

inline void vector_Rect_to_Mat(vector<Rect>& v_rect, Mat& mat)
{
    mat = Mat(v_rect, true);
}

jstring CStr2Jstring( JNIEnv* env, const char* pat )
{
	if (strlen(pat) > 0)
	{
	LOGE("&&&&&&&&&&&&&&&&&&&&&&&&&&&: %s", pat);
	 //����java String�� strClass
	 jclass strClass = (env)->FindClass("java/lang/String");
	 //��ȡjava String�෽��String(byte[],String)�Ĺ�����,���ڽ�����byte[]����ת��Ϊһ����String
	 LOGE("&&&&&&&&&&&&&&***********&&&1&&");
	 jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([B)V");
	 LOGE("&&&&&&&&&&&&&&***********&&2&&&");
	 //����byte����
	 jbyteArray bytes = (env)->NewByteArray((jsize)strlen(pat));
	 LOGE("&&&&&&&&&&&&&&***********&&3&&&");
	 //��char* ת��Ϊbyte����
	 (env)->SetByteArrayRegion(bytes, 0, (jsize)strlen(pat), (jbyte*)pat);
	 LOGE("&&&&&&&&&&&&&&***********&&4&&&");
	 //����String, ������������,����byte����ת����Stringʱ�Ĳ���
	 jstring encoding = (env)->NewStringUTF("utf-8");
	 //��byte����ת��Ϊjava String,�����
	 LOGE("&&&&&&&&&&&&&&***********&&&&5&&");
	 return (jstring)(env)->NewObject(strClass, ctorID, bytes);
	}
	else
	{
		//����java String�� strClass
			 jclass strClass = (env)->FindClass("java/lang/String");
			 //��ȡjava String�෽��String(byte[],String)�Ĺ�����,���ڽ�����byte[]����ת��Ϊһ����String
			 jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "()V");
			 LOGE("&&&&&&&@@@&&&***********&&&&&&");
			 return (jstring)(env)->NewObject(strClass, ctorID);
	}

}

/*
 * Class:     com_baidu_trafficplaterecon_TrafficplateRecon
 * Method:    nativeCreateObject
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_baidu_trafficplaterecon_TrafficplateRecon_nativeCreateObject
  (JNIEnv * jenv, jclass obj)
{
    LOGD("Java_com_baidu_trafficplaterecon_TrafficplateRecon_nativeCreateObject enter");
    jlong result = 0;

    try
    {
    	char rootDir[200];
		getExternalRootDir (jenv, obj, rootDir);

		string dataPath = string(rootDir) + "/lisenceplate/";
		string config_file = dataPath + "clp.conf";

		LOGE(dataPath.c_str());

    	carPlateDetect *det = new carPlateDetect();
    	det->init(config_file.c_str());

        result = (jlong)det;
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeCreateObject caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeCreateObject()");
        return 0;
    }

    LOGD("Java_com_baidu_trafficplaterecon_TrafficplateRecon_nativeCreateObject exit");
    return result;
}

/*
 * Class:     com_baidu_trafficplaterecon_TrafficplateRecon
 * Method:    nativeDestroyObject
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_baidu_trafficplaterecon_TrafficplateRecon_nativeDestroyObject
  (JNIEnv *jenv, jclass, jlong thiz)
{
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDestroyObject enter");
    try
    {
        if(thiz != 0)
        {
            delete (carPlateDetect*)thiz;
        }
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeestroyObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeDestroyObject caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code of DetectionBasedTracker.nativeDestroyObject()");
    }
    LOGD("Java_org_opencv_samples_facedetect_DetectionBasedTracker_nativeDestroyObject exit");
}

/*
 * Class:     com_baidu_trafficplaterecon_TrafficplateRecon
 * Method:    nativeDetect
 * Signature: (JJ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_baidu_trafficplaterecon_TrafficplateRecon_nativeDetect
  (JNIEnv *jenv, jclass, jlong thiz, jlong imageGray)
{
	string plateNumber;
    LOGD("Java_com_baidu_trafficplaterecon_TrafficplateRecon_nativeDetect enter");
    try
    {
    	Mat gray = *((Mat*)imageGray);
    	if (gray.channels() == 1)
    	{
    		plateNumber = ((carPlateDetect*)thiz)->detect(gray);
    	}
    	else
    	{
    		LOGE("image is not gray");
    	}
    }
    catch(cv::Exception& e)
    {
        LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
        jclass je = jenv->FindClass("org/opencv/core/CvException");
        if(!je)
            je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, e.what());
    }
    catch (...)
    {
        LOGD("nativeDetect caught unknown exception");
        jclass je = jenv->FindClass("java/lang/Exception");
        jenv->ThrowNew(je, "Unknown exception in JNI code DetectionBasedTracker.nativeDetect()");
    }
    LOGD("Java_com_baidu_trafficplaterecon_TrafficplateRecon_nativeDetect exit");

    LOGE(plateNumber.c_str());

    return CStr2Jstring(jenv, plateNumber.c_str());
}



