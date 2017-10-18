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
    config.cngMode = AecmFalse;
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

#include <webrtc/common_audio/signal_processing/include/real_fft.h>
#include <math.h>
#include <memory.h>

static int16_t fftout[1024+2];
static int16_t spcm[1024];

static void demo2(JNIEnv *env, jobject jo, jobject buf, jint len){
	unsigned char* input = (unsigned char*)env->GetDirectBufferAddress(buf);
    float RC = 1.0/(500*2*3.14); 
    float dt = 1.0/16000; 
    float alpha = dt/(RC+dt);
	float * output=new float[len];
	float v = ((input[1]<<8) | input[0]);
    output[0] = input[0]*(1.0f/32768.0f);
    for(int i = 1; i < len; ++i)
    { 
		v=((input[i*2+1]<<8) | input[i*2]);
        output[i] = output[i-1] + (alpha*(v - output[i-1]));
    }
	for(int i = 0; i < len; i++){
		short v = (short)(output[i] * 32768);
		input[i*2+1] = (unsigned char)((v&0xFF00)>>8);
		input[i*2] = (unsigned char)(v&0xFF);
	}
	delete[] output;
}

static void demo3(JNIEnv *env, jobject jo, jobject buf, jint len){
	unsigned char* input = (unsigned char*)env->GetDirectBufferAddress(buf);
	struct RealFFT* fft=WebRtcSpl_CreateRealFFT(10);
	float binWidth = 16000/1024;//binIdx 0:DC, binIdx 512:8000hz

	for(int seg=0;seg<len/1024;seg++){
		for(int j=0;j<1024;j++){
			spcm[j]=(input[seg*2048+j*2+1]<<8)|input[seg*2048+j*2];
		}
		int err = WebRtcSpl_RealForwardFFT(fft, (const int16_t*)&spcm[0], (int16_t*)&fftout[0]);
		if(err!=0){
			LOGI("error WebRtcSpl_RealForwardFFT");
			WebRtcSpl_FreeRealFFT(fft);
			return;
		}
	
		
		for(int i = 1; i < 512; i++){
			int freq=binWidth*i;
			//float gain = 1.0 / (sqrt((1 + pow(freq / 500, 20.0 ))));//Butterworth cutoff 500
			//fftout[i]=gain*fftout[i];
			//fftout[1024-i]=gain*fftout[1024-i];
			if(freq>1000){
				fftout[i]*=0.01;
				fftout[1024-i]*=0.01;
			}
		}
		
		err = WebRtcSpl_RealInverseFFT(fft, (const int16_t*)&fftout[0], (int16_t*)&input[seg*2048]);
		if(err!=0){
			LOGI("error WebRtcSpl_RealInverseFFT");
			WebRtcSpl_FreeRealFFT(fft);
			return;
		}
	}
	WebRtcSpl_FreeRealFFT(fft);
}

/*
 * FIR filter class, by Mike Perkins
 * 
 * a simple C++ class for linear phase FIR filtering
 *
 * For background, see the post http://www.cardinalpeak.com/blog?p=1841
 *
 * Copyright (c) 2013, Cardinal Peak, LLC.  http://www.cardinalpeak.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1) Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2) Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 
 * 3) Neither the name of Cardinal Peak nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * CARDINAL PEAK, LLC BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define MAX_NUM_FILTER_TAPS 1000

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

enum filterType {LPF, HPF, BPF};

class Filter{
	private:
		filterType m_filt_t;
		int m_num_taps;
		int m_error_flag;
		double m_Fs;
		double m_Fx;
		double m_lambda;
		double *m_taps;
		double *m_sr;
		void designLPF();
		void designHPF();

		// Only needed for the bandpass filter case
		double m_Fu, m_phi;
		void designBPF();

	public:
		Filter(filterType filt_t, int num_taps, double Fs, double Fx);
		Filter(filterType filt_t, int num_taps, double Fs, double Fl, double Fu);
		~Filter( );
		void init();
		double do_sample(double data_sample);
		int get_error_flag(){return m_error_flag;};
		void get_taps( double *taps );
		int write_taps_to_file( char* filename );
		int write_freqres_to_file( char* filename );
};

#define ECODE(x) {m_error_flag = x; return;}

// Handles LPF and HPF case
Filter::Filter(filterType filt_t, int num_taps, double Fs, double Fx)
{
	m_error_flag = 0;
	m_filt_t = filt_t;
	m_num_taps = num_taps;
	m_Fs = Fs;
	m_Fx = Fx;
	m_lambda = M_PI * Fx / (Fs/2);

	if( Fs <= 0 ) ECODE(-1);
	if( Fx <= 0 || Fx >= Fs/2 ) ECODE(-2);
	if( m_num_taps <= 0 || m_num_taps > MAX_NUM_FILTER_TAPS ) ECODE(-3);

	m_taps = m_sr = NULL;
	m_taps = (double*)malloc( m_num_taps * sizeof(double) );
	m_sr = (double*)malloc( m_num_taps * sizeof(double) );
	if( m_taps == NULL || m_sr == NULL ) ECODE(-4);
	
	init();

	if( m_filt_t == LPF ) designLPF();
	else if( m_filt_t == HPF ) designHPF();
	else ECODE(-5);

	return;
}

// Handles BPF case
Filter::Filter(filterType filt_t, int num_taps, double Fs, double Fl,
               double Fu)
{
	m_error_flag = 0;
	m_filt_t = filt_t;
	m_num_taps = num_taps;
	m_Fs = Fs;
	m_Fx = Fl;
	m_Fu = Fu;
	m_lambda = M_PI * Fl / (Fs/2);
	m_phi = M_PI * Fu / (Fs/2);

	if( Fs <= 0 ) ECODE(-10);
	if( Fl >= Fu ) ECODE(-11);
	if( Fl <= 0 || Fl >= Fs/2 ) ECODE(-12);
	if( Fu <= 0 || Fu >= Fs/2 ) ECODE(-13);
	if( m_num_taps <= 0 || m_num_taps > MAX_NUM_FILTER_TAPS ) ECODE(-14);

	m_taps = m_sr = NULL;
	m_taps = (double*)malloc( m_num_taps * sizeof(double) );
	m_sr = (double*)malloc( m_num_taps * sizeof(double) );
	if( m_taps == NULL || m_sr == NULL ) ECODE(-15);
	
	init();

	if( m_filt_t == BPF ) designBPF();
	else ECODE(-16);

	return;
}

Filter::~Filter()
{
	if( m_taps != NULL ) free( m_taps );
	if( m_sr != NULL ) free( m_sr );
}

void 
Filter::designLPF()
{
	int n;
	double mm;

	for(n = 0; n < m_num_taps; n++){
		mm = n - (m_num_taps - 1.0) / 2.0;
		if( mm == 0.0 ) m_taps[n] = m_lambda / M_PI;
		else m_taps[n] = sin( mm * m_lambda ) / (mm * M_PI);
	}

	return;
}

void 
Filter::designHPF()
{
	int n;
	double mm;

	for(n = 0; n < m_num_taps; n++){
		mm = n - (m_num_taps - 1.0) / 2.0;
		if( mm == 0.0 ) m_taps[n] = 1.0 - m_lambda / M_PI;
		else m_taps[n] = -sin( mm * m_lambda ) / (mm * M_PI);
	}

	return;
}

void 
Filter::designBPF()
{
	int n;
	double mm;

	for(n = 0; n < m_num_taps; n++){
		mm = n - (m_num_taps - 1.0) / 2.0;
		if( mm == 0.0 ) m_taps[n] = (m_phi - m_lambda) / M_PI;
		else m_taps[n] = (   sin( mm * m_phi ) -
		                     sin( mm * m_lambda )   ) / (mm * M_PI);
	}

	return;
}

void 
Filter::get_taps( double *taps )
{
	int i;

	if( m_error_flag != 0 ) return;

	for(i = 0; i < m_num_taps; i++) taps[i] = m_taps[i];

  return;		
}

int 
Filter::write_taps_to_file( char *filename )
{
	FILE *fd;

	if( m_error_flag != 0 ) return -1;

	int i;
	fd = fopen(filename, "w");
	if( fd == NULL ) return -1;

	fprintf(fd, "%d\n", m_num_taps);
	for(i = 0; i < m_num_taps; i++){
		fprintf(fd, "%15.6f\n", m_taps[i]);
	}
	fclose(fd);

	return 0;
}

// Output the magnitude of the frequency response in dB
#define NP 1000
int 
Filter::write_freqres_to_file( char *filename )
{
	FILE *fd;
	int i, k;
	double w, dw;
	double y_r[NP], y_i[NP], y_mag[NP];
	double mag_max = -1;
	double tmp_d;

	if( m_error_flag != 0 ) return -1;

	dw = M_PI / (NP - 1.0);
	for(i = 0; i < NP; i++){
		w = i*dw;
		y_r[i] = y_i[i] = 0;
		for(k = 0; k < m_num_taps; k++){
			y_r[i] += m_taps[k] * cos(k * w);
			y_i[i] -= m_taps[k] * sin(k * w);
		}
	}

	for(i = 0; i < NP; i++){
		y_mag[i] = sqrt( y_r[i]*y_r[i] + y_i[i]*y_i[i] );
		if( y_mag[i] > mag_max ) mag_max = y_mag[i];
	}

	if( mag_max <= 0.0 ) return -2;

	fd = fopen(filename, "w");
	if( fd == NULL ) return -3;

	for(i = 0; i < NP; i++){
		w = i*dw;
		if( y_mag[i] == 0 ) tmp_d = -100;
		else{
			tmp_d = 20 * log10( y_mag[i] / mag_max );
			if( tmp_d < -100 ) tmp_d = -100;
		}
		fprintf(fd, "%10.6e %10.6e\n", w * (m_Fs/2)/M_PI, tmp_d);
	}

	fclose(fd);
	return 0;
}

void 
Filter::init()
{
	int i;

	if( m_error_flag != 0 ) return;

	for(i = 0; i < m_num_taps; i++) m_sr[i] = 0;

	return;
}

double 
Filter::do_sample(double data_sample)
{
	int i;
	double result;

	if( m_error_flag != 0 ) return(0);

	for(i = m_num_taps - 1; i >= 1; i--){
		m_sr[i] = m_sr[i-1];
	}	
	m_sr[0] = data_sample;

	result = 0;
	for(i = 0; i < m_num_taps; i++) result += m_sr[i] * m_taps[i];

	return result;
}

static void demo(JNIEnv *env, jobject jo, jobject buf, jint len){
	//unsigned char* input=(unsigned char*)env->GetDirectBufferAddress(buf);
	unsigned short* input=(unsigned short*)env->GetDirectBufferAddress(buf);
	Filter* my_filter = new Filter(LPF, 51, 16, 0.5);
	for(int i=0;i<len;i++){
		short v = input[i];//(input[i*2+1]<<8 | input[i*2]);
		short v2 = (short)my_filter->do_sample( (double)v  );		
		//input[i*2+1]=v2<<8;
		//input[i*2]=v2&0xFF;
		input[i]=v2;
	}
	delete my_filter;
}

extern "C"{
	JNIEXPORT jlong JNICALL Java_com_webrtc_Aec_create(JNIEnv *env, jobject jo){return create(env,jo);}
	JNIEXPORT void JNICALL Java_com_webrtc_Aec_destory(JNIEnv *env, jobject jo, jlong h){destory(env,jo,h);}
	JNIEXPORT jint JNICALL Java_com_webrtc_Aec_bufferNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer){return bufferNonSayingVoice(env,jo,h,tenMsByteBuffer);}
	JNIEXPORT jint JNICALL Java_com_webrtc_Aec_cancelNonSayingVoice(JNIEnv *env, jobject jo, jlong h, jobject tenMsByteBuffer, jobject outbb, jint delay, jint skew){return cancelNonSayingVoice(env,jo,h,tenMsByteBuffer,outbb,delay,skew);}
	JNIEXPORT void JNICALL Java_com_webrtc_Aec_demo(JNIEnv *env, jobject jo, jobject buf, jint len){/*return demo(env,jo,buf,len);*/}
	JNIEXPORT void JNICALL Java_com_webrtc_Lpfilter_LPF(JNIEnv *env, jobject jo, jobject buf, jint len){demo(env,jo,buf,len);}
}


