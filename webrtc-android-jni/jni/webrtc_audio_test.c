//
//  webrtc_audio_test.c
//  
//
//  Created by apple on 2017/7/17.
//
//

 #include "webrtc_audio_test.h"

// int WebRtcAecTest()
// {
// #define  SAMPLE_RATE 16000
// #define  NN (SAMPLE_RATE/100)
//     short far_frame[NN];
//     short near_frame[NN];
//     short out_frame[NN];

//     float ffar_frame[NN];
//     float fnear_frame[NN];
//     float fout_frame[NN];
    
//     void *aecmInst = NULL;
//     FILE *fp_far  = fopen("speaker.pcm", "rb");
//     FILE *fp_near = fopen("micin.pcm", "rb");
//     FILE *fp_out  = fopen("out_aec.pcm", "wb");
    
// 	printf("----1---\n");
//     do
//     {
//         if(!fp_far || !fp_near || !fp_out)
//         {
//             printf("WebRtcAecTest open file err \n");
//             break;
//         }
        
// 		printf("----2---\n");
// 		aecmInst =  WebRtcAec_Create();
// 		printf("----3--- aecmInst=%p\n", aecmInst);
//         WebRtcAec_Init(aecmInst, SAMPLE_RATE, SAMPLE_RATE);
// 		printf("----4---\n");
        
//         AecConfig config;
//         config.nlpMode = kAecNlpConservative;
//         WebRtcAec_set_config(aecmInst, config);
// 		printf("----1---\n");


//         
//         float* pnear = (float*)fnear_frame;
//         const float* const*  ptr_near = &pnear;//------------------
//         float* pout = (float*)fout_frame;
//         float* const*  ptr_out = &pout;
//         
//         const float* pfar = (const float*)ffar_frame;
//         while(1)
//         {
// 			printf("----1---\n");
//             //speaker.pcm
//             if (NN == fread((void*)far_frame, sizeof(short), NN, fp_far))
//             {
// 				int i = 0;
// 				for(; i< NN; i++)
// 				{
// 					ffar_frame[i] = far_frame[i];
// 				}
				
// 				printf("----2---\n");
//                 fread(near_frame, sizeof(short), NN, fp_near);
// 				i = 0;
// 				for(; i< NN; i++)
// 				{
// 					fnear_frame[i] =near_frame[i];
// 				}
//                 WebRtcAec_BufferFarend(aecmInst, pfar, NN);//对参考声音(回声)的处理
                
// 				printf("----3---\n");
//                 WebRtcAec_Process(aecmInst, ptr_near, 1, ptr_out, NN,200,0);//回声消除
// 				printf("----3.5---\n");
// 				i = 0;
// 				for(; i< NN; i++)
// 				{
// 					out_frame[i] = (short)fout_frame[i];
// 				}
//                 fwrite(out_frame, sizeof(short), NN, fp_out);
// 				printf("----4---\n");
//             }
//             else
//             {
// 				printf("----5---\n");
//                 break;
//             }
//         }
//     } while (0);
    
//     fclose(fp_far);
//     fclose(fp_near);
//     fclose(fp_out);
//     WebRtcAec_Free(aecmInst);
//     return 0;
// }









// //降噪部分
// //参数：
// int nsTest(char *filename, char *outfilename, int mode)
// {
//     //alloc
//     FILE *infp=fopen(filename,"r");
//     int nBands = 1;
//     int frameSize = 160;//10ms对应于160个short


//     short *temp = (short*)malloc(frameSize*sizeof(short));

//     float **pData = (float**)malloc(nBands*sizeof(float*));

//     pData[0] = (float*)malloc(frameSize*sizeof(float));

//     float **pOutData = (float**)malloc(nBands*sizeof(float*));

//     pOutData[0] = (float*)malloc(frameSize*sizeof(float));
    
//     int offset = 0;

//     //init
//     NsHandle *handle = 0;
//     int ret = 0;
//     handle=WebRtcNs_Create();
//     ret= WebRtcNs_Init(handle,16000);
//     if(ret==-1){
//         printf("%s","init failure!");
//         return -1;
//     }
//     ret = WebRtcNs_set_policy(handle,2);

//     //process
//     FILE *outfp = fopen(outfilename,"w");
//     int len = frameSize;
//     while(len > 0)
//     {
//         memset(temp, 0, frameSize*sizeof(short));
//         len = fread(temp, sizeof(short), frameSize, infp);
//         int i;
//         for(i = 0; i < frameSize; i++)
//         {
//             pData[0][i] = (float)temp[i];
//             //fprintf(stdout, "ns:%d\n", temp[i]);
//         }
//         WebRtcNs_AnalyzeCore(handle,pData[0]);
//         WebRtcNs_Process(handle,pData,nBands,pOutData);

//         for(i = 0; i < frameSize; i++)
//         {
//             temp[i] = (short)pOutData[0][i];
//             //fprintf(stderr, "ns:%d\n", temp[i]);
//         }
//         //write file
//         len = fwrite(temp,sizeof(short),len,outfp);
//     }
//     fclose(infp);
//     fclose(outfp);

//     WebRtcNs_Free(handle);
//     free(temp);
//     free(pData[0]);
//     free(pData);
//     free(pOutData[0]);
//     free(pOutData);
// }

// int main()
// {
//     WebRtcAecTest();
//     //NoiseSuppression("out_aec.pcm", "out_aec_ns.pcm", 16000, 2);
//     nsTest("out_aec.pcm", "out_aec_ns.pcm", 2);
//     printf("---------end-------------\n");


//     webrt_SetCconfig(16000, 200);
//     webrtc_Init();

// }
// 
#define Debug  1
//#define Debug_

#ifdef JNI
#include <jni.h> 
#include <Android/log.h>
#define  TAG    "jni log"
#define LOGI(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#else
#define LOGI(...) printf(__VA_ARGS__)
#endif



#define NS_MODE 2

int SAMPLE_RATE = 0;
int WEBRTC_DELAYS = 0;
int NN = 0;
//aec
void *aecmInst = NULL;
//ns
NsHandle *handle = 0;
//agc
void *agcInst = NULL;

float* pfspeaker = NULL;
float* pfmincin = NULL;
float* pout = NULL;
float* pOutData[1] = { NULL };

//#define Debug


void webrtc_Init()
{
    //aec
    aecmInst =  WebRtcAec_Create();
    if (!aecmInst) 
    {
        perror("WebRtcAec instance Create failed!");
        //free(aecmInst);
        //free(handle);
        return;
    }
    WebRtcAec_Init(aecmInst, SAMPLE_RATE, SAMPLE_RATE);
    AecConfig config;
    config.nlpMode = kAecNlpConservative;
    WebRtcAec_set_config( aecmInst, config);

    handle=WebRtcNs_Create();
    if (!handle)
    {
        perror("WebRtcNs instance Create failed!");
        //free(handle);
        WebRtcAec_Free(aecmInst);
        return;
    }
    //ns
    if (WebRtcNs_Init( handle, SAMPLE_RATE) < 0)
    {
        perror("WebRtcNs_Init failed!");
        WebRtcAec_Free(aecmInst);
        WebRtcNs_Free(handle);
        return;
    }
    if (WebRtcNs_set_policy(handle,2) < 0)
    {
        perror("WebRtcNs_set_policy failed!");
        WebRtcAec_Free(aecmInst);
        WebRtcNs_Free(handle);
        return;
    }


    //malloc some mem
    pfspeaker = malloc(sizeof(float)* NN);
    pfmincin = malloc(sizeof(float)* NN);
    pout = (float*)malloc(sizeof(float)*NN);
    pOutData[0] = (float*)malloc(NN*sizeof(float));
}

void webrtc_Destory()
{
    WebRtcAec_Free(aecmInst);
    WebRtcNs_Free(handle);
    free(pfspeaker);
    free(pfmincin);
    free(pout);
    free(pOutData[0]);
 }


//#define Debug_

#ifdef Debug_
FILE *fp_raw;
FILE *fp_aec;

#endif
//note:
//seample_rate: 8k  16k or 24k
//delays
//NN:buffer len
void webrtc_SetCconfig(int seample_rate, int delays)
{
    SAMPLE_RATE = seample_rate;
    WEBRTC_DELAYS = delays;
    NN = seample_rate/100;

    #ifdef Debug_
    //fp_raw = fopen("/sdcard/raw.pcm", "wb");
    //fp_aec = fopen("/sdcard/out_aec1.pcm", "wb");
    #endif
}




int webrtc_Aec_ns(char* speaker_buffer, int speaker_len,char*micin_buffer, int micin_len,char* out_buffer)
{
    if ((!speaker_buffer) || (!micin_buffer)|| (!out_buffer) || (speaker_len != NN*2) || (micin_len != NN*2))
    {
        printf("%s\n", "Error here!");
        perror("invalid arguments");
        return -1;
    }
    //AEC
    //强制使用NN
    //pointer
    //float* pfspeaker = malloc(sizeof(float)* NN);
    memset(pfspeaker, 0, NN);
    memset(pfmincin, 0, NN);

    #ifdef Debug_

    //fwrite(speaker_buffer, 1, speaker_len, fp_raw);

    #endif

    if(!pfspeaker)
        printf("pfspeaker is NULL\n");
    int i;
    for( i = 0;i < NN; i++)
    {
        pfspeaker[i] = ((short*)speaker_buffer)[i];
    }

    //float* pfmincin = malloc(sizeof(float)* NN);
    for(i = 0; i < NN; i++)
    {
        pfmincin[i] =  ((short*)micin_buffer)[i];
    }

    const float* pspeaker_buffer = (const float*)pfspeaker;
    float* pmincin = (float*)pfmincin;
    const float* const*  ptr_near = &pmincin;
    //存放消回音结果
    //float* pout = (float*)malloc(NN);
    float* const*  ptr_out = &pout;

    WebRtcAec_BufferFarend(aecmInst, pspeaker_buffer, NN);//对参考声音(回声)的处理
    printf("%d, %d\n", NN, WEBRTC_DELAYS);
    WebRtcAec_Process(aecmInst, ptr_near, 1, ptr_out, NN, WEBRTC_DELAYS, 0);//回声消除
//#define Debug
//ns
//
#ifdef Debug_
    short* aec_out = malloc(sizeof(short) * NN);
    for(i =0; i < NN; i++)
    {
        aec_out[i] = pout[i];
    }        
    fwrite(aec_out, sizeof(short), NN, fp_aec);
    free(aec_out);
#endif
    int nBands = 1;
    float* pData[1] = { NULL };
    pData[0] = pout;
    WebRtcNs_AnalyzeCore(handle,pData[0]);
    WebRtcNs_Process(handle,pData,nBands,pOutData);

    for(i = 0; i < NN; i++)
    {
        ((short*)out_buffer)[i] = pOutData[0][i];
    }
    return 0;
}

void webrtc_agc_init()
{
    WebRtcAgc_Create(&agcInst);
    if(!agcInst)
    {
        perror("WebRtcAgc_Create failed!");
        return;
    }


    int minLevel = 0;
    int maxLevel = 255;
    int agcMode  = kAgcModeAdaptiveDigital;
    int fs       = 16000;
    int status   = 0; 
    status = WebRtcAgc_Init(agcInst, minLevel, maxLevel, agcMode, fs);
    if(status != 0)
    {
        printf("failed in WebRtcAgc_Init\n");
        return;
    }
    WebRtcAgc_config_t agcConfig;
    agcConfig.compressionGaindB = 20;//在Fixed模式下，越大声音越大
    agcConfig.limiterEnable = 1;
    agcConfig.targetLevelDbfs = 3;   //dbfs表示相对于full scale的下降值，0表示full scale，越小声音越大
    status = WebRtcAgc_set_config(agcInst, agcConfig);
    if(status != 0)
    {
        printf("failed in WebRtcAgc_set_config\n");
        return;
    }
}

void webrtc_agc(char *in, int length, char *out, int simple)
{

    
    //int frameSize = 160;//10ms对应于160个short
    frameSize = simple;
    int micLevelOut = 0;
    if(length != sizeof(short)* frameSize)
    {
        perror("invalid data length");
        return;
    }

    int inMicLevel  = micLevelOut;
    int outMicLevel = 0;
    uint8_t saturationWarning;
    int status = 0;

    status = WebRtcAgc_Process(agcInst, (const WebRtc_Word16*)in, NULL, frameSize, (WebRtc_Word16*)out, NULL, inMicLevel, &outMicLevel, 0, &saturationWarning);
    if (status != 0)
    {
        printf("failed in WebRtcAgc_Process\n");
        return;
    }
    if (saturationWarning == 1)
    {
        printf("saturationWarning\n");
    }
}

void webrtc_agc_destory()
{
    WebRtcAgc_Free(agcInst);
}



#if 0
void webrtc_agc(char *filename, char *outfilename, int mode)
{
    //init agc
    void *agcInst = NULL;

    WebRtcAgc_Create(&agcInst);
    {
        printf("%s: %p\n", "Create failed", agcInst);
        if(!agcInst)
            return;
    }

    int minLevel = 0;
    int maxLevel = 255;
    int agcMode  = kAgcModeAdaptiveDigital;
    int fs       = 16000;
    int status   = 0; 
    printf("%s\n", "1");
    status = WebRtcAgc_Init(agcInst, minLevel, maxLevel, agcMode, fs);
    if(status != 0)
    {
        printf("failed in WebRtcAgc_Init\n");
        return;
    }
    printf("%s\n", "2");
    WebRtcAgc_config_t agcConfig;
    agcConfig.compressionGaindB = 20;//在Fixed模式下，越大声音越大
    agcConfig.limiterEnable = 1;
    agcConfig.targetLevelDbfs = 3;   //dbfs表示相对于full scale的下降值，0表示full scale，越小声音越大
    status = WebRtcAgc_set_config(agcInst, agcConfig);
    printf("%s\n", "3");
    if(status != 0)
    {
        printf("failed in WebRtcAgc_set_config\n");
        return;
    }


    //alloc
    FILE *infp=fopen(filename,"rb");
    if(!infp)
    {
        printf("%s\n", "infp is null");
        return;
    }
    int nBands = 1;
    int frameSize = 160;//10ms对应于160个short
    short **pData = (short**)malloc(nBands*sizeof(short*));
    pData[0] = (short*)malloc(frameSize*sizeof(short));
    short **pOutData = (short**)malloc(nBands*sizeof(short*));
    pOutData[0] = (short*)malloc(frameSize*sizeof(short));

    //process
    FILE *outfp = fopen(outfilename,"wb");
    if(!outfp)
    {
        printf("%s\n", "outfp is null");
        return;
    }
    int len = frameSize;
    int micLevelIn = 0;
    int micLevelOut = 0;
    printf("%s\n", "6");
    while(len > 0)
    {
        memset(pData[0], 0, frameSize*sizeof(short));
        len = fread(pData[0], sizeof(short), frameSize, infp);
        printf("len: %d\n", len);

        int inMicLevel  = micLevelOut;
        int outMicLevel = 0;
        uint8_t saturationWarning;
        printf("%s\n", "4");
        status = WebRtcAgc_Process(agcInst, pData[0], NULL, frameSize, pOutData[0], NULL, inMicLevel, &outMicLevel, 0, &saturationWarning);
        printf("%s\n", "5");
        if (status != 0)
        {
            printf("failed in WebRtcAgc_Process\n");
            return;
        }
        if (saturationWarning == 1)
        {
            printf("saturationWarning\n");
        }
        micLevelIn = outMicLevel;
        printf("%s\n", "7");
        //write file
        len = fwrite(pOutData[0], sizeof(short), len, outfp);
        printf("%s: len: %d\n", "8", len);
    }
    fclose(infp);
    fclose(outfp);

    WebRtcAgc_Free(agcInst);
    free(pData[0]);
    free(pData);
    free(pOutData[0]);
    free(pOutData);
}

#endif



#if Debug
int main()
{
    #if 0
    webrtc_SetCconfig(16000, 200);
    webrtc_Init();
    FILE *speaker_infp=fopen("speaker.pcm","rb");
    FILE *micin_infp=fopen("micin.pcm","rb");
    FILE *fp_out  = fopen("out_aec_ns1.pcm", "wb");

    if(!fp_out)
    {
        printf("%s\n", "out file touch failed");
        return 0;
    }

    //fp_aec = fopen("out_aec1.pcm", "wb");
    if(!speaker_infp || !micin_infp || !fp_out)
    {
        printf("%s\n", "open failed\n");
    }

    char* speaker_buffer = malloc(NN*sizeof(short));
    char* micin_buffer = malloc(NN*sizeof(short));
    char* out_buffer = malloc(NN*sizeof(short));

        while(1)
        {
            memset(speaker_buffer, 0, NN*sizeof(short));
            memset(micin_buffer, 0, NN*sizeof(short));

            if (NN == fread(speaker_buffer, sizeof(short), NN, speaker_infp))
            {
                printf("%s\n", "read file");
                fread(micin_buffer, sizeof(short), NN, micin_infp);
                printf("%s\n", "read file pcm");
                webrtc_Aec_ns(speaker_buffer, NN*2, micin_buffer, NN*2, out_buffer);
                printf("%s\n", "write file pcm");
                fwrite(out_buffer, sizeof(short), NN, fp_out);
            }
            else
            {
                break;
            }
        }
    #endif

    webrtc_agc_init();
    #define NN 160

    FILE *agc_raw=fopen("out_aec_ns1.pcm","rb");
    FILE *agc_out=fopen("out_aec_ns_agc.pcm","wb+");
    if(agc_out)
        printf("%s\n", "agc_out is NULL");

    char* agc_buffer = malloc(sizeof(short)*NN);
    char* agc_out_buffer = malloc(sizeof(short)*NN);

    while(1)
    {
        memset(agc_buffer, 0, NN*sizeof(short));
        memset(agc_out_buffer, 0, NN*sizeof(short));

        if (NN == fread(agc_buffer, sizeof(short), NN, agc_raw))
        {
                webrtc_agc(agc_buffer, NN*sizeof(short),agc_out_buffer, NN);
                
                printf("%s\n", "write file pcm");
                fwrite(agc_out_buffer, sizeof(short), NN, agc_out);
                printf("%s\n", "AAA");
        }
        else
        {
                break;
        }
    }


    fclose(agc_raw);
    fclose(agc_out);

    // printf("%s\n", "success!!!");
    // fclose(speaker_infp);
    // fclose(micin_infp);
    // fclose(fp_out);
    // //fclose(fp_aec);
    // free(speaker_buffer);
    // free(micin_buffer);
    // free(out_buffer);
    //webrtc_Destory();
}
#endif

