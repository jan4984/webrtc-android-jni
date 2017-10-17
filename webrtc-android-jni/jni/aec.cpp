#define AECM

#include <jni.h>
#ifdef AECM
	#include <webrtc/modules/audio_processing/aecm/include/echo_control_mobile.h>
#else
	#include <webrtc/modules/audio_processing/aec/include/echo_cancellation.h>
#endif


#include <Android/log.h>
#ifdef TAG
	#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#else
	#define LOGI(...) (void)0;
#endif



static jlong create(JNIEnv *env, jobject jo){
	#ifdef AECM
	void* ret = WebRtcAecm_Create();
	#else
	void* ret = WebRtcAec_Create();
	#endif
	if(ret == NULL){
		return 0l;
	}
	if(sizeof(ret) > sizeof(jlong)){
		return 0l;
	}
	#ifdef AECM
	WebRtcAecm_Init(ret, 16000);
	AecmConfig config;
    config.cngMode = AecmTrue;
	config.echoMode = 1;
    WebRtcAecm_set_config( ret, config);
	#else
	WebRtcAec_Init(ret, 16000, 16000);
	AecConfig config;
    config.nlpMode = kAecNlpConservative;
    WebRtcAec_set_config( ret, config);
	#endif
	return (jlong)ret;
}

static void destory(JNIEnv *env, jobject jo, jlong h){
	void* p=(void*)h;
	#ifdef AECM
	WebRtcAecm_Free(p);
	#else
	WebRtcAec_Free(p);
	#endif
}

#ifdef AECM
#else
static float fpcm[160];
static float fpcmOut[160];
static float* pfpcm = fpcm;
static float* pfpcmOut = fpcmOut;
#endif

static jint bufferNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer){
	unsigned char* spcm = (unsigned char*)env->GetDirectBufferAddress(tenMsByteBuffer);
	
	void* p=(void*)h;
	#ifdef AECM
	jint ret = (jint)WebRtcAecm_BufferFarend(p,(const short*)spcm,160);
	#else
	//convert to float
	for(int i = 0; i < 160; i++){
		fpcm[i]=(1.0f/32768.0f) * ((spcm[i*2+1]<<8) | spcm[i*2]);
	}
	jint ret = (jint)WebRtcAec_BufferFarend(p, pfpcm, 160);
	#endif
	return ret;
}


static jint cancelNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer, jobject outbb, jint delay, jint skew){
	unsigned char* spcm = (unsigned char*)env->GetDirectBufferAddress(tenMsByteBuffer);
	unsigned char* spcmOut = (unsigned char*)env->GetDirectBufferAddress(outbb);
	
	void* p=(void*)h;
	#ifdef AECM
	jint ret = WebRtcAecm_Process(p,(const int16_t*)spcm,NULL,(int16_t*)spcmOut,160,delay);
	#else
	for(int i = 0; i < 160; i++){
		fpcm[i]=(1.0f/32768.0f) * ((spcm[i*2+1]<<8) | spcm[i*2]);
		if(fpcm[i]>1) fpcm[i] = 1;
		if(fpcm[i]<-1) fpcm[i] = -1;
	}
	LOGI("out addr:%p with this %p",fpcmOut, p);
	jint ret = (jint)WebRtcAec_Process(p, (const float * const*)&pfpcm, 1, (float* const*)&pfpcmOut, 160, delay, skew);
	LOGI("aec this-cancelNonSayingVoice:%p",p);
	if(ret == 0){
		for(int i = 0; i < 160; i++){
			short v = (short)(fpcmOut[i] * 32768);
			spcmOut[i*2+1] = (unsigned char)((v&0xFF00)>>8);
			spcmOut[i*2] = (unsigned char)(v&0xFF);
		}
	}
	#endif
	return ret;
}

extern "C"{
	JNIEXPORT jlong JNICALL Java_com_webrtc_Aec_create(JNIEnv *env, jobject jo){return create(env,jo);}
	JNIEXPORT void JNICALL Java_com_webrtc_Aec_destory(JNIEnv *env, jobject jo, jlong h){destory(env,jo,h);}
	JNIEXPORT jint JNICALL Java_com_webrtc_Aec_bufferNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer){return bufferNonSayingVoice(env,jo,h,tenMsByteBuffer);}
	JNIEXPORT jint JNICALL Java_com_webrtc_Aec_cancelNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer, jobject outbb, jint delay, jint skew){return cancelNonSayingVoice(env,jo,h,tenMsByteBuffer,outbb,delay,skew);}
}