#include <jni.h>
#include <webrtc/modules/audio_processing/aec/include/echo_cancellation.h>

static jlong create(JNIEnv *env, jobject jo){
	void* ret = WebRtcAec_Create();
	if(ret == NULL){
		return 0l;
	}
	WebRtcAec_Init(ret, 16000, 16000);
	AecConfig config;
    config.nlpMode = kAecNlpConservative;
    WebRtcAec_set_config( ret, config);
	return (jlong)ret;
}

static void destory(JNIEnv *env, jobject jo, jlong h){
	WebRtcAec_Free((void*)h);
}

static jint bufferNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer){
	unsigned short* spcm = (unsigned short*)env->GetDirectBufferAddress(tenMsByteBuffer);
	//convert to float
	float* fpcm = new float[160]();
	for(int i = 0; i < 160; i++){
		fpcm[i]=(1.0f/32768.0f) * fpcm[i];
	}

	jint ret = (jint)WebRtcAec_BufferFarend((void*)h, fpcm, 160);
	delete[] fpcm;
	return ret;
}

static jint cancelNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer, jobject outbb, jint delay, jint skew){
	unsigned short* spcm = (unsigned short*)env->GetDirectBufferAddress(tenMsByteBuffer);
	unsigned short* spcmOut = (unsigned short*)env->GetDirectBufferAddress(outbb);
	
	float* fpcm = new float[160]();
	for(int i = 0; i < 160; i++){
		fpcm[i]=(1.0f/32768.0f) * fpcm[i];
		if(fpcm[i]>1) fpcm[i] = 1;
		if(fpcm[i]<-1) fpcm[i] = -1;
	}
	float* fpcmOut= new float[160]();
	
	jint ret = (jint)WebRtcAec_Process((void*)h, (const float * const*)fpcm, 1, (float* const*)fpcmOut, 160, delay, skew);
	if(ret == 0){
		for(int i = 0; i < 160; i++){
			spcmOut[i] = (short)(fpcmOut[i] * 32768);
		}
	}
	delete[] fpcm;
	delete[] fpcmOut;
	return ret;
}

extern "C"{
	JNIEXPORT jlong JNICALL Java_com_webrtc_Aec_create(JNIEnv *env, jobject jo){return create(env,jo);}
	JNIEXPORT void JNICALL Java_com_webrtc_Aec_destory(JNIEnv *env, jobject jo, jlong h){destory(env,jo,h);}
	JNIEXPORT jint JNICALL Java_com_webrtc_Aec_bufferNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer){return bufferNonSayingVoice(env,jo,h,tenMsByteBuffer);}
	JNIEXPORT jint JNICALL Java_com_webrtc_Aec_cancelNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer, jobject outbb, jint delay, jint skew){return cancelNonSayingVoice(env,jo,h,tenMsByteBuffer,outbb,delay,skew);}
}