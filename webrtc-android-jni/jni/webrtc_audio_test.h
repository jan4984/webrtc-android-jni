//
//  webrtc_audio_test.h
//  
//
//  Created by apple on 2017/7/17.
//
//

#ifndef webrtc_audio_test_h
#define webrtc_audio_test_h

#include <stdio.h>
#include "webrtc/modules/audio_processing/ns/nsx_core.h"
#include "webrtc/modules/audio_processing/aec/include/echo_cancellation.h"
#include "webrtc/modules/audio_processing/ns/include/noise_suppression_x.h"

#include "webrtc/common_audio/signal_processing/include/real_fft.h"
#include "webrtc/modules/audio_processing/ns/nsx_defines.h"
#include "webrtc/modules/audio_processing/ns/include/noise_suppression.h"

//
//void webrtc_Init();
//void webrtc_Destory();
//note:
//seample_rate:
//delays
//void webrt_SetCconfig(int seample_rate, int delays);

//void webrtc_Aec_ns();
void webrtc_Init();
void webrtc_Destory();
void webrtc_SetCconfig(int seample_rate, int delays);
int webrtc_Aec_ns(char* speaker_buffer, int speaker_len,char*micin_buffer, int micin_len,char* out_buffer);


#endif /* webrtc_audio_test_h */
