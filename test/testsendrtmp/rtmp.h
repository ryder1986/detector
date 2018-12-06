#ifndef RTMP_H
#define RTMP_H

#include <stdio.h>
#include <stdlib.h>
//#include "librtmp_send264.h"
#include <librtmp/rtmp.h>
#include <librtmp/amf.h>
//#include "librtmp\amf.h"
//#include "sps_decode.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
//定义包头长度，RTMP_MAX_HEADER_SIZE=18
#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)
//存储Nal单元数据的buffer大小
#define BUFFER_SIZE 32768
//搜寻Nal单元时的一些标志
#define GOT_A_NAL_CROSS_BUFFER BUFFER_SIZE+1
#define GOT_A_NAL_INCLUDE_A_BUFFER BUFFER_SIZE+2
#define NO_MORE_BUFFER_TO_READ BUFFER_SIZE+3




/**
 * _RTMPMetadata
 * 内部结构体。该结构体主要用于存储和传递元数据信息
 */
typedef struct _RTMPMetadata
{
    // video, must be h264 type
    unsigned int    nWidth;
    unsigned int    nHeight;
    unsigned int    nFrameRate;
    unsigned int    nSpsLen;
    unsigned char   *Sps;
    unsigned int    nPpsLen;
    unsigned char   *Pps;
} RTMPMetadata,*LPRTMPMetadata;
/**
 * _NaluUnit
 * 内部结构体。该结构体主要用于存储和传递Nal单元的类型、大小和数据
 */
typedef struct _NaluUnit
{
    int type;
    int size;
    unsigned char *data;
}NaluUnit;


#include <string.h>
#include <stdlib.h>
#include <unistd.h>

class RtmpSender{
public:
    RtmpSender()
    {

    }
    ~RtmpSender()
    {

    }
    /**
     * 初始化并连接到服务器
     *
     * @param url 服务器上对应webapp的地址
     *
     * @成功则返回1 , 失败则返回0
     */
    int RTMP264_Connect(const char* url)
    {
        nalhead_pos=0;
        m_nFileBufSize=BUFFER_SIZE;
        m_pFileBuf=(unsigned char*)malloc(BUFFER_SIZE);
        m_pFileBuf_tmp=(unsigned char*)malloc(BUFFER_SIZE);
    //    InitSockets();

        m_pRtmp = RTMP_Alloc();
        RTMP_Init(m_pRtmp);
        /*设置URL*/
        if (RTMP_SetupURL(m_pRtmp,(char*)url) == FALSE)
        {
            RTMP_Free(m_pRtmp);
            return false;
        }
        /*设置可写,即发布流,这个函数必须在连接前使用,否则无效*/
        RTMP_EnableWrite(m_pRtmp);
        /*连接服务器*/
        if (RTMP_Connect(m_pRtmp, NULL) == FALSE)
        {
            RTMP_Free(m_pRtmp);
            return false;
        }

        /*连接流*/
        if (RTMP_ConnectStream(m_pRtmp,0) == FALSE)
        {
            RTMP_Close(m_pRtmp);
            RTMP_Free(m_pRtmp);
            return false;
        }
        return true;
    }

    /**
     * 发送视频的sps和pps信息
     *
     * @param pps 存储视频的pps信息
     * @param pps_len 视频的pps信息长度
     * @param sps 存储视频的pps信息
     * @param sps_len 视频的sps信息长度
     *
     * @成功则返回 1 , 失败则返回0
     */
    int SendVideoSpsPps(unsigned char *pps,int pps_len,unsigned char * sps,int sps_len)
    {
        RTMPPacket * packet=NULL;//rtmp包结构
        unsigned char * body=NULL;
        int i;
        packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+1024);
        //RTMPPacket_Reset(packet);//重置packet状态
        memset(packet,0,RTMP_HEAD_SIZE+1024);
        packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
        body = (unsigned char *)packet->m_body;
        i = 0;
        body[i++] = 0x17;
        body[i++] = 0x00;

        body[i++] = 0x00;
        body[i++] = 0x00;
        body[i++] = 0x00;

        /*AVCDecoderConfigurationRecord*/
        body[i++] = 0x01;
        body[i++] = sps[1];
        body[i++] = sps[2];
        body[i++] = sps[3];
        body[i++] = 0xff;

        /*sps*/
        body[i++]   = 0xe1;
        body[i++] = (sps_len >> 8) & 0xff;
        body[i++] = sps_len & 0xff;
        memcpy(&body[i],sps,sps_len);
        i +=  sps_len;

        /*pps*/
        body[i++]   = 0x01;
        body[i++] = (pps_len >> 8) & 0xff;
        body[i++] = (pps_len) & 0xff;
        memcpy(&body[i],pps,pps_len);
        i +=  pps_len;

        packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
        packet->m_nBodySize = i;
        packet->m_nChannel = 0x04;
        packet->m_nTimeStamp = 0;
        packet->m_hasAbsTimestamp = 0;
        packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
        packet->m_nInfoField2 = m_pRtmp->m_stream_id;

        /*调用发送接口*/
        int nRet = RTMP_SendPacket(m_pRtmp,packet,TRUE);
        free(packet);    //释放内存
        return nRet;
    }

    /**
     * 发送RTMP数据包
     *
     * @param nPacketType 数据类型
     * @param data 存储数据内容
     * @param size 数据大小
     * @param nTimestamp 当前包的时间戳
     *
     * @成功则返回 1 , 失败则返回一个小于0的数
     */
    int SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp)
    {
        RTMPPacket* packet;
        /*分配包内存和初始化,len为包体长度*/
        packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+size);
        memset(packet,0,RTMP_HEAD_SIZE);
        /*包体内存*/
        packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
        packet->m_nBodySize = size;
        memcpy(packet->m_body,data,size);
        packet->m_hasAbsTimestamp = 0;
        packet->m_packetType = nPacketType; /*此处为类型有两种一种是音频,一种是视频*/
        packet->m_nInfoField2 = m_pRtmp->m_stream_id;
        packet->m_nChannel = 0x04;

        packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
        if (RTMP_PACKET_TYPE_AUDIO ==nPacketType && size !=4)
        {
            packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
        }
        packet->m_nTimeStamp = nTimestamp;
        /*发送*/
        int nRet =0;
        if (RTMP_IsConnected(m_pRtmp))
        {
            nRet = RTMP_SendPacket(m_pRtmp,packet,TRUE); /*TRUE为放进发送队列,FALSE是不放进发送队列,直接发送*/
        }
        /*释放内存*/
        free(packet);
        return nRet;
    }


    /**
     * 断开连接，释放相关的资源。
     *
     */
    void RTMP264_Close()
    {
        if(m_pRtmp)
        {
            RTMP_Close(m_pRtmp);
            RTMP_Free(m_pRtmp);
            m_pRtmp = NULL;
        }

        if (m_pFileBuf != NULL)
        {
            free(m_pFileBuf);
        }
        if (m_pFileBuf_tmp != NULL)
        {
            free(m_pFileBuf_tmp);
        }
    }

    /**
     * 发送H264数据帧
     *
     * @param data 存储数据帧内容
     * @param size 数据帧的大小
     * @param bIsKeyFrame 记录该帧是否为关键帧
     * @param nTimeStamp 当前帧的时间戳
     *
     * @成功则返回 1 , 失败则返回0
     */
    int SendH264Packet(unsigned char *data,unsigned int size,int bIsKeyFrame,unsigned int nTimeStamp)
    {
        if(data == NULL && size<11){
            return false;
        }

        unsigned char *body = (unsigned char*)malloc(size+9);
        memset(body,0,size+9);

        int i = 0;
        if(bIsKeyFrame){
            body[i++] = 0x17;// 1:Iframe  7:AVC
            body[i++] = 0x01;// AVC NALU
            body[i++] = 0x00;
            body[i++] = 0x00;
            body[i++] = 0x00;


            // NALU size
            body[i++] = size>>24 &0xff;
            body[i++] = size>>16 &0xff;
            body[i++] = size>>8 &0xff;
            body[i++] = size&0xff;
            // NALU data
            memcpy(&body[i],data,size);
            SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
        }else{
            body[i++] = 0x27;// 2:Pframe  7:AVC
            body[i++] = 0x01;// AVC NALU
            body[i++] = 0x00;
            body[i++] = 0x00;
            body[i++] = 0x00;


            // NALU size
            body[i++] = size>>24 &0xff;
            body[i++] = size>>16 &0xff;
            body[i++] = size>>8 &0xff;
            body[i++] = size&0xff;
            // NALU data
            memcpy(&body[i],data,size);
        }


        int bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO,body,i+size,nTimeStamp);

        free(body);

        return bRet;
    }

    /**
     * 将内存中的一段H.264编码的视频数据利用RTMP协议发送到服务器
     *
     * @param read_buffer 回调函数，当数据不足的时候，系统会自动调用该函数获取输入数据。
     *					2个参数功能：
     *					uint8_t *buf：外部数据送至该地址
     *					int buf_size：外部数据大小
     *					返回值：成功读取的内存大小
     * @成功则返回1 , 失败则返回0
     */
    int RTMP264_Send(int (*read_buffer)(unsigned char *buf, int buf_size))
    {
        int ret;
        uint32_t now,last_update;

        memset(&metaData,0,sizeof(RTMPMetadata));
        memset(m_pFileBuf,0,BUFFER_SIZE);
        if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<0)
        {
            return FALSE;
        }

        NaluUnit naluUnit;
        // 读取SPS帧
        ReadFirstNaluFromBuf(naluUnit,read_buffer);
        metaData.nSpsLen = naluUnit.size;
        metaData.Sps=NULL;
        metaData.Sps=(unsigned char*)malloc(naluUnit.size);
        memcpy(metaData.Sps,naluUnit.data,naluUnit.size);

        // 读取PPS帧
        ReadOneNaluFromBuf(naluUnit,read_buffer);
        metaData.nPpsLen = naluUnit.size;
        metaData.Pps=NULL;
        metaData.Pps=(unsigned char*)malloc(naluUnit.size);
        memcpy(metaData.Pps,naluUnit.data,naluUnit.size);

        // 解码SPS,获取视频图像宽、高信息
        int width = 0,height = 0, fps=0;
        h264_decode_sps(metaData.Sps,metaData.nSpsLen,width,height,fps);
        //metaData.nWidth = width;
        //metaData.nHeight = height;
        if(fps)
            metaData.nFrameRate = fps;
        else
            metaData.nFrameRate = 25;

        //发送PPS,SPS
        //ret=SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
        //if(ret!=1)
        //	return FALSE;

        unsigned int tick = 0;
        unsigned int tick_gap = 1000/metaData.nFrameRate;
        ReadOneNaluFromBuf(naluUnit,read_buffer);
        int bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;
        while(SendH264Packet(naluUnit.data,naluUnit.size,bKeyframe,tick))
        {
    got_sps_pps:
            //if(naluUnit.size==8581)
                printf("NALU size:%8d\n",naluUnit.size);
            last_update=RTMP_GetTime();
            if(!ReadOneNaluFromBuf(naluUnit,read_buffer))
                    goto end;
            if(naluUnit.type == 0x07 || naluUnit.type == 0x08)
                goto got_sps_pps;
            bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;
            tick +=tick_gap;
            now=RTMP_GetTime();
            usleep((tick_gap-now+last_update)*1000);
            //msleep(40);
        }
        end:
        free(metaData.Sps);
        free(metaData.Pps);
        return TRUE;
    }





    typedef  unsigned int UINT;
    typedef  unsigned char BYTE;
    typedef  unsigned long DWORD;
    UINT Ue(BYTE *pBuff, UINT nLen, UINT &nStartBit)
    {
        //计算0bit的个数
        UINT nZeroNum = 0;
        while (nStartBit < nLen * 8)
        {
            if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8))) //&:按位与，%取余
            {
                break;
            }
            nZeroNum++;
            nStartBit++;
        }
        nStartBit ++;


        //计算结果
        DWORD dwRet = 0;
        for (UINT i=0; i<nZeroNum; i++)
        {
            dwRet <<= 1;
            if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
            {
                dwRet += 1;
            }
            nStartBit++;
        }
        return (1 << nZeroNum) - 1 + dwRet;
    }


    int Se(BYTE *pBuff, UINT nLen, UINT &nStartBit)
    {
        int UeVal=Ue(pBuff,nLen,nStartBit);
        double k=UeVal;
        int nValue=ceil(k/2);//ceil函数：ceil函数的作用是求不小于给定实数的最小整数。ceil(2)=ceil(1.2)=cei(1.5)=2.00
        if (UeVal % 2==0)
            nValue=-nValue;
        return nValue;
    }


    DWORD u(UINT BitCount,BYTE * buf,UINT &nStartBit)
    {
        DWORD dwRet = 0;
        for (UINT i=0; i<BitCount; i++)
        {
            dwRet <<= 1;
            if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
            {
                dwRet += 1;
            }
            nStartBit++;
        }
        return dwRet;
    }
    /**
     * H264的NAL起始码防竞争机制
     *
     * @param buf SPS数据内容
     *
     * @无返回值
     */
    void de_emulation_prevention(BYTE* buf,unsigned int* buf_size)
    {
        int i=0,j=0;
        BYTE* tmp_ptr=NULL;
        unsigned int tmp_buf_size=0;
        int val=0;

        tmp_ptr=buf;
        tmp_buf_size=*buf_size;
        for(i=0;i<(tmp_buf_size-2);i++)
        {
            //check for 0x000003
            val=(tmp_ptr[i]^0x00) +(tmp_ptr[i+1]^0x00)+(tmp_ptr[i+2]^0x03);
            if(val==0)
            {
                //kick out 0x03
                for(j=i+2;j<tmp_buf_size-1;j++)
                    tmp_ptr[j]=tmp_ptr[j+1];

                //and so we should devrease bufsize
                (*buf_size)--;
            }
        }

        return;
    }

    /**
     * 解码SPS,获取视频图像宽、高信息
     *
     * @param buf SPS数据内容
     * @param nLen SPS数据的长度
     * @param width 图像宽度
     * @param height 图像高度
     * @成功则返回1 , 失败则返回0
     */



    int h264_decode_sps(BYTE * buf,unsigned int nLen,int &width,int &height,int &fps)
    {
        UINT StartBit=0;
        fps=0;
        de_emulation_prevention(buf,&nLen);

        int forbidden_zero_bit=u(1,buf,StartBit);
        int nal_ref_idc=u(2,buf,StartBit);
        int nal_unit_type=u(5,buf,StartBit);
        if(nal_unit_type==7)
        {
            int profile_idc=u(8,buf,StartBit);
            int constraint_set0_flag=u(1,buf,StartBit);//(buf[1] & 0x80)>>7;
            int constraint_set1_flag=u(1,buf,StartBit);//(buf[1] & 0x40)>>6;
            int constraint_set2_flag=u(1,buf,StartBit);//(buf[1] & 0x20)>>5;
            int constraint_set3_flag=u(1,buf,StartBit);//(buf[1] & 0x10)>>4;
            int reserved_zero_4bits=u(4,buf,StartBit);
            int level_idc=u(8,buf,StartBit);

            int seq_parameter_set_id=Ue(buf,nLen,StartBit);

            if( profile_idc == 100 || profile_idc == 110 ||
                profile_idc == 122 || profile_idc == 144 )
            {
                int chroma_format_idc=Ue(buf,nLen,StartBit);
                if( chroma_format_idc == 3 )
                    int residual_colour_transform_flag=u(1,buf,StartBit);
                int bit_depth_luma_minus8=Ue(buf,nLen,StartBit);
                int bit_depth_chroma_minus8=Ue(buf,nLen,StartBit);
                int qpprime_y_zero_transform_bypass_flag=u(1,buf,StartBit);
                int seq_scaling_matrix_present_flag=u(1,buf,StartBit);

                int seq_scaling_list_present_flag[8];
                if( seq_scaling_matrix_present_flag )
                {
                    for( int i = 0; i < 8; i++ ) {
                        seq_scaling_list_present_flag[i]=u(1,buf,StartBit);
                    }
                }
            }
            int log2_max_frame_num_minus4=Ue(buf,nLen,StartBit);
            int pic_order_cnt_type=Ue(buf,nLen,StartBit);
            if( pic_order_cnt_type == 0 )
                int log2_max_pic_order_cnt_lsb_minus4=Ue(buf,nLen,StartBit);
            else if( pic_order_cnt_type == 1 )
            {
                int delta_pic_order_always_zero_flag=u(1,buf,StartBit);
                int offset_for_non_ref_pic=Se(buf,nLen,StartBit);
                int offset_for_top_to_bottom_field=Se(buf,nLen,StartBit);
                int num_ref_frames_in_pic_order_cnt_cycle=Ue(buf,nLen,StartBit);

                int *offset_for_ref_frame=new int[num_ref_frames_in_pic_order_cnt_cycle];
                for( int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
                    offset_for_ref_frame[i]=Se(buf,nLen,StartBit);
                delete [] offset_for_ref_frame;
            }
            int num_ref_frames=Ue(buf,nLen,StartBit);
            int gaps_in_frame_num_value_allowed_flag=u(1,buf,StartBit);
            int pic_width_in_mbs_minus1=Ue(buf,nLen,StartBit);
            int pic_height_in_map_units_minus1=Ue(buf,nLen,StartBit);

            width=(pic_width_in_mbs_minus1+1)*16;
            height=(pic_height_in_map_units_minus1+1)*16;

            int frame_mbs_only_flag=u(1,buf,StartBit);
            if(!frame_mbs_only_flag)
                int mb_adaptive_frame_field_flag=u(1,buf,StartBit);

            int direct_8x8_inference_flag=u(1,buf,StartBit);
            int frame_cropping_flag=u(1,buf,StartBit);
            if(frame_cropping_flag)
            {
                int frame_crop_left_offset=Ue(buf,nLen,StartBit);
                int frame_crop_right_offset=Ue(buf,nLen,StartBit);
                int frame_crop_top_offset=Ue(buf,nLen,StartBit);
                int frame_crop_bottom_offset=Ue(buf,nLen,StartBit);
            }
            int vui_parameter_present_flag=u(1,buf,StartBit);
            if(vui_parameter_present_flag)
            {
                int aspect_ratio_info_present_flag=u(1,buf,StartBit);
                if(aspect_ratio_info_present_flag)
                {
                    int aspect_ratio_idc=u(8,buf,StartBit);
                    if(aspect_ratio_idc==255)
                    {
                        int sar_width=u(16,buf,StartBit);
                        int sar_height=u(16,buf,StartBit);
                    }
                }
                int overscan_info_present_flag=u(1,buf,StartBit);
                if(overscan_info_present_flag)
                    int overscan_appropriate_flagu=u(1,buf,StartBit);
                int video_signal_type_present_flag=u(1,buf,StartBit);
                if(video_signal_type_present_flag)
                {
                    int video_format=u(3,buf,StartBit);
                    int video_full_range_flag=u(1,buf,StartBit);
                    int colour_description_present_flag=u(1,buf,StartBit);
                    if(colour_description_present_flag)
                    {
                        int colour_primaries=u(8,buf,StartBit);
                        int transfer_characteristics=u(8,buf,StartBit);
                        int matrix_coefficients=u(8,buf,StartBit);
                    }
                }
                int chroma_loc_info_present_flag=u(1,buf,StartBit);
                if(chroma_loc_info_present_flag)
                {
                    int chroma_sample_loc_type_top_field=Ue(buf,nLen,StartBit);
                    int chroma_sample_loc_type_bottom_field=Ue(buf,nLen,StartBit);
                }
                int timing_info_present_flag=u(1,buf,StartBit);
                if(timing_info_present_flag)
                {
                    int num_units_in_tick=u(32,buf,StartBit);
                    int time_scale=u(32,buf,StartBit);
                    fps=time_scale/(2*num_units_in_tick);
                }
            }
            return true;
        }
        else
            return false;
    }
    /**
     * 从内存中读取出第一个Nal单元
     *
     * @param nalu 存储nalu数据
     * @param read_buffer 回调函数，当数据不足的时候，系统会自动调用该函数获取输入数据。
     *					2个参数功能：
     *					uint8_t *buf：外部数据送至该地址
     *					int buf_size：外部数据大小
     *					返回值：成功读取的内存大小
     * @成功则返回 1 , 失败则返回0
     */
    int ReadFirstNaluFromBuf(NaluUnit &nalu,int (*read_buffer)(uint8_t *buf, int buf_size))
    {
        int naltail_pos=nalhead_pos;
        memset(m_pFileBuf_tmp,0,BUFFER_SIZE);
        while(nalhead_pos<m_nFileBufSize)
        {
            //search for nal header
            if(m_pFileBuf[nalhead_pos++] == 0x00 &&
                m_pFileBuf[nalhead_pos++] == 0x00)
            {
                if(m_pFileBuf[nalhead_pos++] == 0x01)
                    goto gotnal_head;
                else
                {
                    //cuz we have done an i++ before,so we need to roll back now
                    nalhead_pos--;
                    if(m_pFileBuf[nalhead_pos++] == 0x00 &&
                        m_pFileBuf[nalhead_pos++] == 0x01)
                        goto gotnal_head;
                    else
                        continue;
                }
            }
            else
                continue;

            //search for nal tail which is also the head of next nal
    gotnal_head:
            //normal case:the whole nal is in this m_pFileBuf
            naltail_pos = nalhead_pos;
            while (naltail_pos<m_nFileBufSize)
            {
                if(m_pFileBuf[naltail_pos++] == 0x00 &&
                    m_pFileBuf[naltail_pos++] == 0x00 )
                {
                    if(m_pFileBuf[naltail_pos++] == 0x01)
                    {
                        nalu.size = (naltail_pos-3)-nalhead_pos;
                        break;
                    }
                    else
                    {
                        naltail_pos--;
                        if(m_pFileBuf[naltail_pos++] == 0x00 &&
                            m_pFileBuf[naltail_pos++] == 0x01)
                        {
                            nalu.size = (naltail_pos-4)-nalhead_pos;
                            break;
                        }
                    }
                }
            }

            nalu.type = m_pFileBuf[nalhead_pos]&0x1f;
            memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu.size);
            nalu.data=m_pFileBuf_tmp;
            nalhead_pos=naltail_pos;
            return TRUE;
        }
    }

    /**
     * 从内存中读取出一个Nal单元
     *
     * @param nalu 存储nalu数据
     * @param read_buffer 回调函数，当数据不足的时候，系统会自动调用该函数获取输入数据。
     *					2个参数功能：
     *					uint8_t *buf：外部数据送至该地址
     *					int buf_size：外部数据大小
     *					返回值：成功读取的内存大小
     * @成功则返回 1 , 失败则返回0
     */
    int ReadOneNaluFromBuf(NaluUnit &nalu,int (*read_buffer)(uint8_t *buf, int buf_size))
    {

        int naltail_pos=nalhead_pos;
        int ret;
        int nalustart;//nal的开始标识符是几个00
        memset(m_pFileBuf_tmp,0,BUFFER_SIZE);
        nalu.size=0;
        while(1)
        {
            if(nalhead_pos==NO_MORE_BUFFER_TO_READ)
                return FALSE;
            while(naltail_pos<m_nFileBufSize)
            {
                //search for nal tail
                if(m_pFileBuf[naltail_pos++] == 0x00 &&
                    m_pFileBuf[naltail_pos++] == 0x00)
                {
                    if(m_pFileBuf[naltail_pos++] == 0x01)
                    {
                        nalustart=3;
                        goto gotnal ;
                    }
                    else
                    {
                        //cuz we have done an i++ before,so we need to roll back now
                        naltail_pos--;
                        if(m_pFileBuf[naltail_pos++] == 0x00 &&
                            m_pFileBuf[naltail_pos++] == 0x01)
                        {
                            nalustart=4;
                            goto gotnal;
                        }
                        else
                            continue;
                    }
                }
                else
                    continue;

                gotnal:
                    /**
                     *special case1:parts of the nal lies in a m_pFileBuf and we have to read from buffer
                     *again to get the rest part of this nal
                     */
                    if(nalhead_pos==GOT_A_NAL_CROSS_BUFFER || nalhead_pos==GOT_A_NAL_INCLUDE_A_BUFFER)
                    {
                        nalu.size = nalu.size+naltail_pos-nalustart;
                        if(nalu.size>BUFFER_SIZE)
                        {
                            m_pFileBuf_tmp_old=m_pFileBuf_tmp;	//// save pointer in case realloc fails
                            if((m_pFileBuf_tmp = (unsigned char*)realloc(m_pFileBuf_tmp,nalu.size)) ==  NULL )
                            {
                                free( m_pFileBuf_tmp_old );  // free original block
                                return FALSE;
                            }
                        }
                        memcpy(m_pFileBuf_tmp+nalu.size+nalustart-naltail_pos,m_pFileBuf,naltail_pos-nalustart);
                        nalu.data=m_pFileBuf_tmp;
                        nalhead_pos=naltail_pos;
                        return TRUE;
                    }
                    //normal case:the whole nal is in this m_pFileBuf
                    else
                    {
                        nalu.type = m_pFileBuf[nalhead_pos]&0x1f;
                        nalu.size=naltail_pos-nalhead_pos-nalustart;
                        if(nalu.type==0x06)
                        {
                            nalhead_pos=naltail_pos;
                            continue;
                        }
                        memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu.size);
                        nalu.data=m_pFileBuf_tmp;
                        nalhead_pos=naltail_pos;
                        return TRUE;
                    }
            }

            if(naltail_pos>=m_nFileBufSize && nalhead_pos!=GOT_A_NAL_CROSS_BUFFER && nalhead_pos != GOT_A_NAL_INCLUDE_A_BUFFER)
            {
                nalu.size = BUFFER_SIZE-nalhead_pos;
                nalu.type = m_pFileBuf[nalhead_pos]&0x1f;
                memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu.size);
                if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
                {
                    memcpy(m_pFileBuf_tmp+nalu.size,m_pFileBuf,ret);
                    nalu.size=nalu.size+ret;
                    nalu.data=m_pFileBuf_tmp;
                    nalhead_pos=NO_MORE_BUFFER_TO_READ;
                    return FALSE;
                }
                naltail_pos=0;
                nalhead_pos=GOT_A_NAL_CROSS_BUFFER;
                continue;
            }
            if(nalhead_pos==GOT_A_NAL_CROSS_BUFFER || nalhead_pos == GOT_A_NAL_INCLUDE_A_BUFFER)
            {
                nalu.size = BUFFER_SIZE+nalu.size;

                    m_pFileBuf_tmp_old=m_pFileBuf_tmp;	//// save pointer in case realloc fails
                    if((m_pFileBuf_tmp = (unsigned char*)realloc(m_pFileBuf_tmp,nalu.size)) ==  NULL )
                    {
                        free( m_pFileBuf_tmp_old );  // free original block
                        return FALSE;
                    }

                memcpy(m_pFileBuf_tmp+nalu.size-BUFFER_SIZE,m_pFileBuf,BUFFER_SIZE);

                if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
                {
                    memcpy(m_pFileBuf_tmp+nalu.size,m_pFileBuf,ret);
                    nalu.size=nalu.size+ret;
                    nalu.data=m_pFileBuf_tmp;
                    nalhead_pos=NO_MORE_BUFFER_TO_READ;
                    return FALSE;
                }
                naltail_pos=0;
                nalhead_pos=GOT_A_NAL_INCLUDE_A_BUFFER;
                continue;
            }
        }
        return FALSE;
    }

private:
    unsigned int  m_nFileBufSize;
    unsigned int  nalhead_pos;
    RTMP* m_pRtmp;
    RTMPMetadata metaData;
    unsigned char *m_pFileBuf;
    unsigned char *m_pFileBuf_tmp;
    unsigned char* m_pFileBuf_tmp_old;	//used for realloc


};
class RtmpSender1{
public:
    RtmpSender1()
    {

    }
    ~RtmpSender1()
    {

    }
    /**
     * 初始化并连接到服务器
     *
     * @param url 服务器上对应webapp的地址
     *
     * @成功则返回1 , 失败则返回0
     */
    int RTMP264_Connect(const char* url)
    {
        nalhead_pos=0;
        m_nFileBufSize=BUFFER_SIZE;
        m_pFileBuf=(unsigned char*)malloc(BUFFER_SIZE);
        m_pFileBuf_tmp=(unsigned char*)malloc(BUFFER_SIZE);
    //    InitSockets();

        m_pRtmp = RTMP_Alloc();
        RTMP_Init(m_pRtmp);
        /*设置URL*/
        if (RTMP_SetupURL(m_pRtmp,(char*)url) == FALSE)
        {
            RTMP_Free(m_pRtmp);
            return false;
        }
        /*设置可写,即发布流,这个函数必须在连接前使用,否则无效*/
        RTMP_EnableWrite(m_pRtmp);
        /*连接服务器*/
        if (RTMP_Connect(m_pRtmp, NULL) == FALSE)
        {
            RTMP_Free(m_pRtmp);
            return false;
        }

        /*连接流*/
        if (RTMP_ConnectStream(m_pRtmp,0) == FALSE)
        {
            RTMP_Close(m_pRtmp);
            RTMP_Free(m_pRtmp);
            return false;
        }
        return true;
    }

    /**
     * 发送视频的sps和pps信息
     *
     * @param pps 存储视频的pps信息
     * @param pps_len 视频的pps信息长度
     * @param sps 存储视频的pps信息
     * @param sps_len 视频的sps信息长度
     *
     * @成功则返回 1 , 失败则返回0
     */
    int SendVideoSpsPps(unsigned char *pps,int pps_len,unsigned char * sps,int sps_len)
    {
        RTMPPacket * packet=NULL;//rtmp包结构
        unsigned char * body=NULL;
        int i;
        packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+1024);
        //RTMPPacket_Reset(packet);//重置packet状态
        memset(packet,0,RTMP_HEAD_SIZE+1024);
        packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
        body = (unsigned char *)packet->m_body;
        i = 0;
        body[i++] = 0x17;
        body[i++] = 0x00;

        body[i++] = 0x00;
        body[i++] = 0x00;
        body[i++] = 0x00;

        /*AVCDecoderConfigurationRecord*/
        body[i++] = 0x01;
        body[i++] = sps[1];
        body[i++] = sps[2];
        body[i++] = sps[3];
        body[i++] = 0xff;

        /*sps*/
        body[i++]   = 0xe1;
        body[i++] = (sps_len >> 8) & 0xff;
        body[i++] = sps_len & 0xff;
        memcpy(&body[i],sps,sps_len);
        i +=  sps_len;

        /*pps*/
        body[i++]   = 0x01;
        body[i++] = (pps_len >> 8) & 0xff;
        body[i++] = (pps_len) & 0xff;
        memcpy(&body[i],pps,pps_len);
        i +=  pps_len;

        packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
        packet->m_nBodySize = i;
        packet->m_nChannel = 0x04;
        packet->m_nTimeStamp = 0;
        packet->m_hasAbsTimestamp = 0;
        packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
        packet->m_nInfoField2 = m_pRtmp->m_stream_id;

        /*调用发送接口*/
        int nRet = RTMP_SendPacket(m_pRtmp,packet,TRUE);
        free(packet);    //释放内存
        return nRet;
    }

    /**
     * 发送RTMP数据包
     *
     * @param nPacketType 数据类型
     * @param data 存储数据内容
     * @param size 数据大小
     * @param nTimestamp 当前包的时间戳
     *
     * @成功则返回 1 , 失败则返回一个小于0的数
     */
    int SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp)
    {
        RTMPPacket* packet;
        /*分配包内存和初始化,len为包体长度*/
        packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+size);
        memset(packet,0,RTMP_HEAD_SIZE);
        /*包体内存*/
        packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
        packet->m_nBodySize = size;
        memcpy(packet->m_body,data,size);
        packet->m_hasAbsTimestamp = 0;
        packet->m_packetType = nPacketType; /*此处为类型有两种一种是音频,一种是视频*/
        packet->m_nInfoField2 = m_pRtmp->m_stream_id;
        packet->m_nChannel = 0x04;

        packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
        if (RTMP_PACKET_TYPE_AUDIO ==nPacketType && size !=4)
        {
            packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
        }
        packet->m_nTimeStamp = nTimestamp;
        /*发送*/
        int nRet =0;
        if (RTMP_IsConnected(m_pRtmp))
        {
        //    if(packet->m_nBodySize>1000){
            nRet = RTMP_SendPacket(m_pRtmp,packet,TRUE); /*TRUE为放进发送队列,FALSE是不放进发送队列,直接发送*/
            //    }
            }//TODO: ryder test
        /*释放内存*/
        free(packet);
        return nRet;
    }


    /**
     * 断开连接，释放相关的资源。
     *
     */
    void RTMP264_Close()
    {
        if(m_pRtmp)
        {
            RTMP_Close(m_pRtmp);
            RTMP_Free(m_pRtmp);
            m_pRtmp = NULL;
        }

        if (m_pFileBuf != NULL)
        {
            free(m_pFileBuf);
        }
        if (m_pFileBuf_tmp != NULL)
        {
            free(m_pFileBuf_tmp);
        }
    }

    /**
     * 发送H264数据帧
     *
     * @param data 存储数据帧内容
     * @param size 数据帧的大小
     * @param bIsKeyFrame 记录该帧是否为关键帧
     * @param nTimeStamp 当前帧的时间戳
     *
     * @成功则返回 1 , 失败则返回0
     */
    int SendH264Packet(unsigned char *data,unsigned int size,int bIsKeyFrame,unsigned int nTimeStamp)
    {
        if(data == NULL && size<11){
            return false;
        }

        unsigned char *body = (unsigned char*)malloc(size+9);
        memset(body,0,size+9);

        int i = 0;
        if(bIsKeyFrame){
            body[i++] = 0x17;// 1:Iframe  7:AVC
            body[i++] = 0x01;// AVC NALU
            body[i++] = 0x00;
            body[i++] = 0x00;
            body[i++] = 0x00;


            // NALU size
            body[i++] = size>>24 &0xff;
            body[i++] = size>>16 &0xff;
            body[i++] = size>>8 &0xff;
            body[i++] = size&0xff;
            // NALU data
            memcpy(&body[i],data,size);
            SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
        }else{
            body[i++] = 0x27;// 2:Pframe  7:AVC
            body[i++] = 0x01;// AVC NALU
            body[i++] = 0x00;
            body[i++] = 0x00;
            body[i++] = 0x00;


            // NALU size
            body[i++] = size>>24 &0xff;
            body[i++] = size>>16 &0xff;
            body[i++] = size>>8 &0xff;
            body[i++] = size&0xff;
            // NALU data
            memcpy(&body[i],data,size);
        }


        int bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO,body,i+size,nTimeStamp);

        free(body);

        return bRet;
    }

    /**
     * 将内存中的一段H.264编码的视频数据利用RTMP协议发送到服务器
     *
     * @param read_buffer 回调函数，当数据不足的时候，系统会自动调用该函数获取输入数据。
     *					2个参数功能：
     *					uint8_t *buf：外部数据送至该地址
     *					int buf_size：外部数据大小
     *					返回值：成功读取的内存大小
     * @成功则返回1 , 失败则返回0
     */


    std::function <int(unsigned char *buf, int buf_size)> read_buffer;
  //  int RTMP264_Send(int (*read_buffer)(unsigned char *buf, int buf_size))
    int RTMP264_Send()

    {
        int ret;
        uint32_t now,last_update;

        memset(&metaData,0,sizeof(RTMPMetadata));
        memset(m_pFileBuf,0,BUFFER_SIZE);
        if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<0)
        {
            return FALSE;
        }

        NaluUnit naluUnit;
        // 读取SPS帧
        ReadFirstNaluFromBuf(naluUnit);
        metaData.nSpsLen = naluUnit.size;
        metaData.Sps=NULL;
        metaData.Sps=(unsigned char*)malloc(naluUnit.size);
        memcpy(metaData.Sps,naluUnit.data,naluUnit.size);

        // 读取PPS帧
        ReadOneNaluFromBuf(naluUnit);


        metaData.nPpsLen = naluUnit.size;
        metaData.Pps=NULL;
        metaData.Pps=(unsigned char*)malloc(naluUnit.size);
        memcpy(metaData.Pps,naluUnit.data,naluUnit.size);

        // 解码SPS,获取视频图像宽、高信息
        int width = 0,height = 0, fps=0;
        h264_decode_sps(metaData.Sps,metaData.nSpsLen,width,height,fps);
        //metaData.nWidth = width;
        //metaData.nHeight = height;
        if(fps)
            metaData.nFrameRate = fps;
        else
            metaData.nFrameRate = 25;

        //发送PPS,SPS
        //ret=SendVideoSpsPps(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
        //if(ret!=1)
        //	return FALSE;

        unsigned int tick = 0;
        unsigned int tick_gap = 1000/metaData.nFrameRate;
        ReadOneNaluFromBuf(naluUnit);
        int bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;


        while(SendH264Packet(naluUnit.data,naluUnit.size,bKeyframe,tick))
        {
    got_sps_pps:
            //if(naluUnit.size==8581)
                printf("NALU size:%8d\n",naluUnit.size);
            last_update=RTMP_GetTime();
            if(!ReadOneNaluFromBuf(naluUnit))
                    goto end;
            if(naluUnit.type == 0x07 || naluUnit.type == 0x08)
                goto got_sps_pps;
            bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;
            tick +=tick_gap;
            now=RTMP_GetTime();
            usleep((tick_gap-now+last_update)*1000);
            //msleep(40);
        }
        end:
        free(metaData.Sps);
        free(metaData.Pps);
        return TRUE;
    }





    typedef  unsigned int UINT;
    typedef  unsigned char BYTE;
    typedef  unsigned long DWORD;
    UINT Ue(BYTE *pBuff, UINT nLen, UINT &nStartBit)
    {
        //计算0bit的个数
        UINT nZeroNum = 0;
        while (nStartBit < nLen * 8)
        {
            if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8))) //&:按位与，%取余
            {
                break;
            }
            nZeroNum++;
            nStartBit++;
        }
        nStartBit ++;


        //计算结果
        DWORD dwRet = 0;
        for (UINT i=0; i<nZeroNum; i++)
        {
            dwRet <<= 1;
            if (pBuff[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
            {
                dwRet += 1;
            }
            nStartBit++;
        }
        return (1 << nZeroNum) - 1 + dwRet;
    }


    int Se(BYTE *pBuff, UINT nLen, UINT &nStartBit)
    {
        int UeVal=Ue(pBuff,nLen,nStartBit);
        double k=UeVal;
        int nValue=ceil(k/2);//ceil函数：ceil函数的作用是求不小于给定实数的最小整数。ceil(2)=ceil(1.2)=cei(1.5)=2.00
        if (UeVal % 2==0)
            nValue=-nValue;
        return nValue;
    }


    DWORD u(UINT BitCount,BYTE * buf,UINT &nStartBit)
    {
        DWORD dwRet = 0;
        for (UINT i=0; i<BitCount; i++)
        {
            dwRet <<= 1;
            if (buf[nStartBit / 8] & (0x80 >> (nStartBit % 8)))
            {
                dwRet += 1;
            }
            nStartBit++;
        }
        return dwRet;
    }
    /**
     * H264的NAL起始码防竞争机制
     *
     * @param buf SPS数据内容
     *
     * @无返回值
     */
    void de_emulation_prevention(BYTE* buf,unsigned int* buf_size)
    {
        int i=0,j=0;
        BYTE* tmp_ptr=NULL;
        unsigned int tmp_buf_size=0;
        int val=0;

        tmp_ptr=buf;
        tmp_buf_size=*buf_size;
        for(i=0;i<(tmp_buf_size-2);i++)
        {
            //check for 0x000003
            val=(tmp_ptr[i]^0x00) +(tmp_ptr[i+1]^0x00)+(tmp_ptr[i+2]^0x03);
            if(val==0)
            {
                //kick out 0x03
                for(j=i+2;j<tmp_buf_size-1;j++)
                    tmp_ptr[j]=tmp_ptr[j+1];

                //and so we should devrease bufsize
                (*buf_size)--;
            }
        }

        return;
    }

    /**
     * 解码SPS,获取视频图像宽、高信息
     *
     * @param buf SPS数据内容
     * @param nLen SPS数据的长度
     * @param width 图像宽度
     * @param height 图像高度
     * @成功则返回1 , 失败则返回0
     */



    int h264_decode_sps(BYTE * buf,unsigned int nLen,int &width,int &height,int &fps)
    {
        UINT StartBit=0;
        fps=0;
        de_emulation_prevention(buf,&nLen);

        int forbidden_zero_bit=u(1,buf,StartBit);
        int nal_ref_idc=u(2,buf,StartBit);
        int nal_unit_type=u(5,buf,StartBit);
        if(nal_unit_type==7)
        {
            int profile_idc=u(8,buf,StartBit);
            int constraint_set0_flag=u(1,buf,StartBit);//(buf[1] & 0x80)>>7;
            int constraint_set1_flag=u(1,buf,StartBit);//(buf[1] & 0x40)>>6;
            int constraint_set2_flag=u(1,buf,StartBit);//(buf[1] & 0x20)>>5;
            int constraint_set3_flag=u(1,buf,StartBit);//(buf[1] & 0x10)>>4;
            int reserved_zero_4bits=u(4,buf,StartBit);
            int level_idc=u(8,buf,StartBit);

            int seq_parameter_set_id=Ue(buf,nLen,StartBit);

            if( profile_idc == 100 || profile_idc == 110 ||
                profile_idc == 122 || profile_idc == 144 )
            {
                int chroma_format_idc=Ue(buf,nLen,StartBit);
                if( chroma_format_idc == 3 )
                    int residual_colour_transform_flag=u(1,buf,StartBit);
                int bit_depth_luma_minus8=Ue(buf,nLen,StartBit);
                int bit_depth_chroma_minus8=Ue(buf,nLen,StartBit);
                int qpprime_y_zero_transform_bypass_flag=u(1,buf,StartBit);
                int seq_scaling_matrix_present_flag=u(1,buf,StartBit);

                int seq_scaling_list_present_flag[8];
                if( seq_scaling_matrix_present_flag )
                {
                    for( int i = 0; i < 8; i++ ) {
                        seq_scaling_list_present_flag[i]=u(1,buf,StartBit);
                    }
                }
            }
            int log2_max_frame_num_minus4=Ue(buf,nLen,StartBit);
            int pic_order_cnt_type=Ue(buf,nLen,StartBit);
            if( pic_order_cnt_type == 0 )
                int log2_max_pic_order_cnt_lsb_minus4=Ue(buf,nLen,StartBit);
            else if( pic_order_cnt_type == 1 )
            {
                int delta_pic_order_always_zero_flag=u(1,buf,StartBit);
                int offset_for_non_ref_pic=Se(buf,nLen,StartBit);
                int offset_for_top_to_bottom_field=Se(buf,nLen,StartBit);
                int num_ref_frames_in_pic_order_cnt_cycle=Ue(buf,nLen,StartBit);

                int *offset_for_ref_frame=new int[num_ref_frames_in_pic_order_cnt_cycle];
                for( int i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
                    offset_for_ref_frame[i]=Se(buf,nLen,StartBit);
                delete [] offset_for_ref_frame;
            }
            int num_ref_frames=Ue(buf,nLen,StartBit);
            int gaps_in_frame_num_value_allowed_flag=u(1,buf,StartBit);
            int pic_width_in_mbs_minus1=Ue(buf,nLen,StartBit);
            int pic_height_in_map_units_minus1=Ue(buf,nLen,StartBit);

            width=(pic_width_in_mbs_minus1+1)*16;
            height=(pic_height_in_map_units_minus1+1)*16;

            int frame_mbs_only_flag=u(1,buf,StartBit);
            if(!frame_mbs_only_flag)
                int mb_adaptive_frame_field_flag=u(1,buf,StartBit);

            int direct_8x8_inference_flag=u(1,buf,StartBit);
            int frame_cropping_flag=u(1,buf,StartBit);
            if(frame_cropping_flag)
            {
                int frame_crop_left_offset=Ue(buf,nLen,StartBit);
                int frame_crop_right_offset=Ue(buf,nLen,StartBit);
                int frame_crop_top_offset=Ue(buf,nLen,StartBit);
                int frame_crop_bottom_offset=Ue(buf,nLen,StartBit);
            }
            int vui_parameter_present_flag=u(1,buf,StartBit);
            if(vui_parameter_present_flag)
            {
                int aspect_ratio_info_present_flag=u(1,buf,StartBit);
                if(aspect_ratio_info_present_flag)
                {
                    int aspect_ratio_idc=u(8,buf,StartBit);
                    if(aspect_ratio_idc==255)
                    {
                        int sar_width=u(16,buf,StartBit);
                        int sar_height=u(16,buf,StartBit);
                    }
                }
                int overscan_info_present_flag=u(1,buf,StartBit);
                if(overscan_info_present_flag)
                    int overscan_appropriate_flagu=u(1,buf,StartBit);
                int video_signal_type_present_flag=u(1,buf,StartBit);
                if(video_signal_type_present_flag)
                {
                    int video_format=u(3,buf,StartBit);
                    int video_full_range_flag=u(1,buf,StartBit);
                    int colour_description_present_flag=u(1,buf,StartBit);
                    if(colour_description_present_flag)
                    {
                        int colour_primaries=u(8,buf,StartBit);
                        int transfer_characteristics=u(8,buf,StartBit);
                        int matrix_coefficients=u(8,buf,StartBit);
                    }
                }
                int chroma_loc_info_present_flag=u(1,buf,StartBit);
                if(chroma_loc_info_present_flag)
                {
                    int chroma_sample_loc_type_top_field=Ue(buf,nLen,StartBit);
                    int chroma_sample_loc_type_bottom_field=Ue(buf,nLen,StartBit);
                }
                int timing_info_present_flag=u(1,buf,StartBit);
                if(timing_info_present_flag)
                {
                    int num_units_in_tick=u(32,buf,StartBit);
                    int time_scale=u(32,buf,StartBit);
                    fps=time_scale/(2*num_units_in_tick);
                }
            }
            return true;
        }
        else
            return false;
    }
    /**
     * 从内存中读取出第一个Nal单元
     *
     * @param nalu 存储nalu数据
     * @param read_buffer 回调函数，当数据不足的时候，系统会自动调用该函数获取输入数据。
     *					2个参数功能：
     *					uint8_t *buf：外部数据送至该地址
     *					int buf_size：外部数据大小
     *					返回值：成功读取的内存大小
     * @成功则返回 1 , 失败则返回0
     */
    int ReadFirstNaluFromBuf(NaluUnit &nalu)
    {
        int naltail_pos=nalhead_pos;
        memset(m_pFileBuf_tmp,0,BUFFER_SIZE);
        while(nalhead_pos<m_nFileBufSize)
        {
            //search for nal header
            if(m_pFileBuf[nalhead_pos++] == 0x00 &&
                m_pFileBuf[nalhead_pos++] == 0x00)
            {
                if(m_pFileBuf[nalhead_pos++] == 0x01)
                    goto gotnal_head;
                else
                {
                    //cuz we have done an i++ before,so we need to roll back now
                    nalhead_pos--;
                    if(m_pFileBuf[nalhead_pos++] == 0x00 &&
                        m_pFileBuf[nalhead_pos++] == 0x01)
                        goto gotnal_head;
                    else
                        continue;
                }
            }
            else
                continue;

            //search for nal tail which is also the head of next nal
    gotnal_head:
            //normal case:the whole nal is in this m_pFileBuf
            naltail_pos = nalhead_pos;
            while (naltail_pos<m_nFileBufSize)
            {
                if(m_pFileBuf[naltail_pos++] == 0x00 &&
                    m_pFileBuf[naltail_pos++] == 0x00 )
                {
                    if(m_pFileBuf[naltail_pos++] == 0x01)
                    {
                        nalu.size = (naltail_pos-3)-nalhead_pos;
                        break;
                    }
                    else
                    {
                        naltail_pos--;
                        if(m_pFileBuf[naltail_pos++] == 0x00 &&
                            m_pFileBuf[naltail_pos++] == 0x01)
                        {
                            nalu.size = (naltail_pos-4)-nalhead_pos;
                            break;
                        }
                    }
                }
            }

            nalu.type = m_pFileBuf[nalhead_pos]&0x1f;
            memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu.size);
            nalu.data=m_pFileBuf_tmp;
            nalhead_pos=naltail_pos;
            return TRUE;
        }
    }

    /**
     * 从内存中读取出一个Nal单元
     *
     * @param nalu 存储nalu数据
     * @param read_buffer 回调函数，当数据不足的时候，系统会自动调用该函数获取输入数据。
     *					2个参数功能：
     *					uint8_t *buf：外部数据送至该地址
     *					int buf_size：外部数据大小
     *					返回值：成功读取的内存大小
     * @成功则返回 1 , 失败则返回0
     */
    int ReadOneNaluFromBuf(NaluUnit &nalu)
    {

        int naltail_pos=nalhead_pos;
        int ret;
        int nalustart;//nal的开始标识符是几个00
        memset(m_pFileBuf_tmp,0,BUFFER_SIZE);

        nalu.size=0;// WHY


        while(1)
        {
            if(nalhead_pos==NO_MORE_BUFFER_TO_READ)
                return FALSE;
            while(naltail_pos<m_nFileBufSize)
            {
                //search for nal tail
                if(m_pFileBuf[naltail_pos++] == 0x00 &&
                    m_pFileBuf[naltail_pos++] == 0x00)
                {
                    if(m_pFileBuf[naltail_pos++] == 0x01)
                    {
                        nalustart=3;
                        goto gotnal ;
                    }
                    else
                    {
                        //cuz we have done an i++ before,so we need to roll back now
                        naltail_pos--;
                        if(m_pFileBuf[naltail_pos++] == 0x00 &&
                            m_pFileBuf[naltail_pos++] == 0x01)
                        {
                            nalustart=4;
                            goto gotnal;
                        }
                        else
                            continue;
                    }
                }
                else
                    continue;

                gotnal:
                    /**
                     *special case1:parts of the nal lies in a m_pFileBuf and we have to read from buffer
                     *again to get the rest part of this nal
                     */
                    if(nalhead_pos==GOT_A_NAL_CROSS_BUFFER || nalhead_pos==GOT_A_NAL_INCLUDE_A_BUFFER)
                    {
                        nalu.size = nalu.size+naltail_pos-nalustart;
                        if(nalu.size>BUFFER_SIZE)
                        {
                            m_pFileBuf_tmp_old=m_pFileBuf_tmp;	//// save pointer in case realloc fails
                            if((m_pFileBuf_tmp = (unsigned char*)realloc(m_pFileBuf_tmp,nalu.size)) ==  NULL )
                            {
                                free( m_pFileBuf_tmp_old );  // free original block
                                return FALSE;
                            }
                        }
                        memcpy(m_pFileBuf_tmp+nalu.size+nalustart-naltail_pos,m_pFileBuf,naltail_pos-nalustart);
                        nalu.data=m_pFileBuf_tmp;
                        nalhead_pos=naltail_pos;
                        return TRUE;
                    }
                    //normal case:the whole nal is in this m_pFileBuf
                    else
                    {
                        nalu.type = m_pFileBuf[nalhead_pos]&0x1f;
                        nalu.size=naltail_pos-nalhead_pos-nalustart;
                        if(nalu.type==0x06)
                        {
                            nalhead_pos=naltail_pos;
                            continue;
                        }
                        memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu.size);
                        nalu.data=m_pFileBuf_tmp;
                        nalhead_pos=naltail_pos;
                        return TRUE;
                    }
            }

            if(naltail_pos>=m_nFileBufSize && nalhead_pos!=GOT_A_NAL_CROSS_BUFFER && nalhead_pos != GOT_A_NAL_INCLUDE_A_BUFFER)
            {
                nalu.size = BUFFER_SIZE-nalhead_pos;
                nalu.type = m_pFileBuf[nalhead_pos]&0x1f;
                memcpy(m_pFileBuf_tmp,m_pFileBuf+nalhead_pos,nalu.size);
                if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
                {
                    memcpy(m_pFileBuf_tmp+nalu.size,m_pFileBuf,ret);
                    nalu.size=nalu.size+ret;
                    nalu.data=m_pFileBuf_tmp;
                    nalhead_pos=NO_MORE_BUFFER_TO_READ;
                    return FALSE;
                }
                naltail_pos=0;
                nalhead_pos=GOT_A_NAL_CROSS_BUFFER;
                continue;
            }
            if(nalhead_pos==GOT_A_NAL_CROSS_BUFFER || nalhead_pos == GOT_A_NAL_INCLUDE_A_BUFFER)
            {
                nalu.size = BUFFER_SIZE+nalu.size;

                    m_pFileBuf_tmp_old=m_pFileBuf_tmp;	//// save pointer in case realloc fails
                    if((m_pFileBuf_tmp = (unsigned char*)realloc(m_pFileBuf_tmp,nalu.size)) ==  NULL )
                    {
                        free( m_pFileBuf_tmp_old );  // free original block
                        return FALSE;
                    }

                memcpy(m_pFileBuf_tmp+nalu.size-BUFFER_SIZE,m_pFileBuf,BUFFER_SIZE);

                if((ret=read_buffer(m_pFileBuf,m_nFileBufSize))<BUFFER_SIZE)
                {
                    memcpy(m_pFileBuf_tmp+nalu.size,m_pFileBuf,ret);
                    nalu.size=nalu.size+ret;
                    nalu.data=m_pFileBuf_tmp;
                    nalhead_pos=NO_MORE_BUFFER_TO_READ;
                    return FALSE;
                }
                naltail_pos=0;
                nalhead_pos=GOT_A_NAL_INCLUDE_A_BUFFER;
                continue;
            }
        }
        return FALSE;
    }

private:
    unsigned int  m_nFileBufSize;
    unsigned int  nalhead_pos;
    RTMP* m_pRtmp;
    RTMPMetadata metaData;
    unsigned char *m_pFileBuf;
    unsigned char *m_pFileBuf_tmp;
    unsigned char* m_pFileBuf_tmp_old;	//used for realloc


};

#endif // TEST_H

