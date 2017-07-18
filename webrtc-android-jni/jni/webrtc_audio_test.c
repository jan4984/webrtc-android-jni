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



#define NS_MODE 2

int SAMPLE_RATE = 0;
int WEBRTC_DELAYS = 0;
int NN = 0;
void *aecmInst = NULL;
NsHandle *handle = 0;

float* pfspeaker = NULL;
float* pfmincin = NULL;
float* pout = NULL;
float* pOutData[1] = { NULL };



void webrtc_Init()
{
    aecmInst =  WebRtcAec_Create();
    if (!aecmInst) 
    {
        perror("WebRtcAec instance Create failed!");
        free(aecmInst);
        free(handle);
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
        free(handle);
        WebRtcAec_Free(aecmInst);
        return;
    }
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
    pout = (float*)malloc(NN);
    pOutData[0] = (float*)malloc(NN*sizeof(float));
}

void webrtc_Destory()
{
    WebRtcAec_Free(aecmInst);
    WebRtcNs_Free(handle);
    free(pfspeaker);
    free(pfmincin);
    free(pout);
    free(pOutData);
 }

//note:
//seample_rate: 8k  16k or 24k
//delays
//NN:buffer len
void webrtc_SetCconfig(int seample_rate, int delays)
{
    SAMPLE_RATE = seample_rate;
    WEBRTC_DELAYS = delays;
    NN = seample_rate/100;
}


FILE *fp_aec;

int webrtc_Aec_ns(char* speaker_buffer, int speaker_len,char*micin_buffer, int micin_len,char* out_buffer)
{
    if (!speaker_buffer || !micin_buffer || speaker_len != NN || micin_len != NN)
    {
        perror("invalid arguments");
        return -1;
    }
    //AEC
    //强制使用NN
    //pointer
    //float* pfspeaker = malloc(sizeof(float)* NN);
    memset(pfspeaker, 0, NN);
    memset(pfmincin, 0, NN);

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
#ifdef Debug
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



int main()
{
    webrtc_SetCconfig(16000, 200);
    webrtc_Init();
    FILE *speaker_infp=fopen("speaker.pcm","rb");
    FILE *micin_infp=fopen("micin.pcm","rb");
    FILE *fp_out  = fopen("out_aec_ns1.pcm", "wb");

    fp_aec = fopen("out_aec1.pcm", "wb");
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
                fread(micin_buffer, sizeof(short), NN, micin_infp);
                webrtc_Aec_ns(speaker_buffer, NN, micin_buffer, NN, out_buffer);
                fwrite(out_buffer, sizeof(short), NN, fp_out);
            }
            else
            {
                break;
            }
        }
    printf("%s\n", "success!!!");
    fclose(speaker_infp);
    fclose(micin_infp);
    fclose(fp_out);
    fclose(fp_aec);
    free(speaker_buffer);
    free(micin_buffer);
    free(out_buffer);
    webrtc_Destory();
}

