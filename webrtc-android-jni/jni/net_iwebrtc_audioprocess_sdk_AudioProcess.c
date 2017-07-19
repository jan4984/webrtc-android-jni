//
//  net_iwebrtc_audioprocess_sdk_AudioProcess.c
//  
//
//  Created by apple on 2017/7/19.
//
//

#include <stdio.h>
#include "net_iwebrtc_audioprocess_sdk_AudioProcess.h"
#include "webrtc_audio_test.h"

#define Debug

#ifdef Debug
FILE *fp_raw;
FILE *fp_aec_ns;
#endif

//fake
JNIEXPORT jint JNICALL Java_net_iwebrtc_audioprocess_sdk_AudioProcess_create
(JNIEnv * Env, jobject obj)
{
    return 0;
}


JNIEXPORT jint JNICALL Java_net_iwebrtc_audioprocess_sdk_AudioProcess_set_1config
(JNIEnv *Env, jobject obj, jint jsample_rate, jint jdelays)
{
    int sample_rate = jsample_rate;
    int delays = jdelays;
    webrtc_SetCconfig(sample_rate, delays);

    #ifdef Debug
    fp_raw = fopen("/sdcard/raw.pcm", "wb");
    fp_aec_ns = fopen("/sdcard/aec_ns.pcm", "wb");
    #endif

    return 0;
}


JNIEXPORT jint JNICALL Java_net_iwebrtc_audioprocess_sdk_AudioProcess_init(JNIEnv *Env, jobject obj)
{
    webrtc_Init();
    return 0;
}

JNIEXPORT jboolean JNICALL Java_net_iwebrtc_audioprocess_sdk_AudioProcess_processStream10msData(JNIEnv *env, jobject obj, jbyteArray speaker_buffer, jint speaker_len, jbyteArray micin_buffer, jint minin_len, jbyteArray out)
{
    if(speaker_len != minin_len)
        return JNI_FALSE;
    char* speaker_data = NULL;
    char* minic_data = NULL;
    char* out_buffer = NULL;
    //Log.e("jni:", "process");


    speaker_data = (char*)(*env)->GetByteArrayElements(env, speaker_buffer, 0);
    minic_data = (char*)(*env)->GetByteArrayElements(env, micin_buffer, 0);
    out_buffer = (char*)(*env)->GetByteArrayElements(env, out, 0);

    #ifdef Debug
    fwrite(speaker_data, speaker_len, 1, fp_raw);
    #endif


    int ret = 0;
    ret = webrtc_Aec_ns(speaker_data, speaker_len, minic_data, minin_len, out_buffer);

    #ifdef Debug
    fwrite(out_buffer, speaker_len, 1, fp_aec_ns);
    #endif

    if(ret < 0)
        return JNI_FALSE;
    (*env)->SetByteArrayRegion(env, out, 0, speaker_len, (char *)out_buffer);

    free(speaker_data);
    free(minic_data);
    free(out_buffer);
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_net_iwebrtc_audioprocess_sdk_AudioProcess_destroy(JNIEnv * Env, jobject obj)
{
    webrtc_Destory();
    return JNI_TRUE;
}




