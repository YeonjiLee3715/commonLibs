//
// Created by 이연지 on 2017-06-01.
//

#include "CVideoStreamer.h"

#include "CLogger.h"

#ifdef DEBUG_MODE
#include "cmnTypeDef.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/imgutils.h>
#ifdef __cplusplus
}
#endif

namespace nsCVideoStreamer {
    const char* const TAG = "nsCVideoStreamer";
}

using namespace nsCVideoStreamer;

CVideoStreamer::CVideoStreamer( )
: m_fullWidth(0), m_fullHeight(0), m_type(0), m_multiplier(0), m_fileType(nsCmn::FILE_TYPE_CAM)
, m_pFormatCtx(NULL), m_pCodecCtx(NULL), m_pFrame(NULL), m_pFrameRGB(NULL)
, m_videoStream(0), m_frameNumber(0)
#ifdef DEBUG_MODE
, m_selection(0)
#endif
{

}

bool CVideoStreamer::init( )
{
    bool isSuccess = false;

    isSuccess = true;



    setStatus( isSuccess );
    return isSuccess;
}

bool CVideoStreamer::openFile( const char* fileName )
{
    bool isSuccess = false;
    int ret;
    int err;
    int i;
    AVCodec *pCodec;
    uint8_t *buffer;
    int numBytes;

    av_register_all();
    LOGI(TAG, "Registered formats", "" );

    err = avformat_open_input(&m_pFormatCtx, fileName, NULL, NULL);
    LOGI(TAG, "Called open file", "" );

    if(err != 0)
    {
        LOGE(TAG, "Couldn't open %s. error(%d): %s", fileName, err, av_err2str(err) );
        return isSuccess;
    }

    LOGI(TAG, "Opened file %s", fileName);

    if(avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
    {
        LOGE(TAG, "Unable to get stream info", "" );
        return isSuccess;
    }

    m_videoStream = -1;
    for (i=0; i < m_pFormatCtx->nb_streams; i++)
    {
        if( m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_videoStream = i;
            break;
        }
    }
    if( m_videoStream == -1 )
    {
        LOGE(TAG, "Unable to find video stream", "" );
        return isSuccess;
    }

    LOGI(TAG, "Video stream is %d", m_videoStream );

    m_pCodecCtx = m_pFormatCtx->streams[m_videoStream]->codec;

    pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
    if(pCodec == NULL)
    {
        LOGE(TAG, "Unsupported codec", "" );
        return isSuccess;
    }

    if(avcodec_open2(m_pCodecCtx, pCodec, NULL) < 0)
    {
        LOGE(TAG, "Unable to open codec", "" );
        return isSuccess;
    }

    m_pFrame = av_frame_alloc();
    m_pFrameRGB = av_frame_alloc();
    LOGI( TAG, "Video size is [%d x %d]", m_pCodecCtx->width, m_pCodecCtx->height);

    numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height, 1 );
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    avpicture_fill((AVPicture *)m_pFrameRGB, buffer, AV_PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height);

#ifdef DEBUG_MODE
    do {
        std::string filePath(fileName);
        int pos = filePath.find_last_of('.');
        if (pos == std::string::npos) {
            LOGE(TAG, "Wrong file. filePath: %s", filePath.c_str());
            break;
        }

        ++pos; //skip '.'

        std::string strExtension = filePath.substr(pos, (filePath.size() - pos) - 1);

        if (nsCmn::compare(strExtension.c_str(), "mp4", false))
            m_selection = (int) nsCmn::FILE_TYPE_MP4;
        else if (nsCmn::compare(strExtension.c_str(), "avi", false))
            m_selection = (int) nsCmn::FILE_TYPE_AVI;
    }while(false);
#endif

    isSuccess = setFrameInfo();
    m_frameNumber = 0;

    setStatus( isSuccess );

    return isSuccess;
}

// The flush packet is a non-NULL packet with size 0 and data NULL
bool CVideoStreamer::read( cv::OutputArray output )
{
    bool isSuccess = false;
    int err = 0;
    AVPacket packet;

    while(isSuccess == false)
    {
        err = av_read_frame(m_pFormatCtx, &packet);
        if (err < 0 && err != AVERROR(EAGAIN) && err != AVERROR_EOF)
        {
            LOGE(TAG, "Failed read frame, error(%d): %s", err, av_err2str(err));
            break;
        }

        if(packet.stream_index == m_videoStream)
        {
            err = avcodec_send_packet(m_pCodecCtx, &packet);
            if (err < 0)
            {
                LOGE(TAG, "Failed to send packet, error(%d): %s", err, av_err2str(err));
                break;
            }

            err = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
            if (err < 0 && err != AVERROR(EAGAIN) && err != AVERROR_EOF)
            {
                LOGE(TAG, "Failed get frame, error(%d): %s", err, av_err2str(err));
                break;
            }

            struct SwsContext *img_convert_ctx;
            img_convert_ctx = sws_getCachedContext(NULL, m_pCodecCtx->width, m_pCodecCtx->height,
                                                   m_pCodecCtx->pix_fmt, m_pCodecCtx->width,
                                                   m_pCodecCtx->height, AV_PIX_FMT_RGB24,
                                                   SWS_BICUBIC, NULL, NULL, NULL);
            sws_scale(img_convert_ctx, (const uint8_t *const *) ((AVPicture *) m_pFrame)->data,
                      ((AVPicture *) m_pFrame)->linesize, 0, m_pCodecCtx->height,
                      ((AVPicture *) m_pFrameRGB)->data, ((AVPicture *) m_pFrameRGB)->linesize);

#if defined(OPENCV_3_2_0)
            output.assign(
                    cv::Mat(m_pCodecCtx->height, m_pCodecCtx->width, CV_8UC3, m_pFrameRGB->data[0],
                            m_pFrameRGB->linesize[0]));
#endif
            sws_freeContext(img_convert_ctx);

            isSuccess = true;
        }

        av_free_packet(&packet);
    }

    ++m_frameNumber;

    return isSuccess;
}

#ifdef DEBUG_MODE
bool CVideoStreamer::readAt(cv::OutputArray output, int frameId)
{
    if(m_selection > (int)nsCmn::FILE_TYPE_CAM)
    {
        if( seekFrame(frameId) == true )
            return read(output);
        else
            return false;
    }
    else
        LOGE( TAG, "can not use this function with CAM mode", "" );

    return false;
}

bool CVideoStreamer::readAfter(cv::OutputArray output, time_t msec)
{
    if(m_selection > (int)nsCmn::FILE_TYPE_CAM)
    {
        if( seekFrame( time_t( getMsTimeStamp() ) + msec ) )
            return read(output);
        else
            return false;
    }
    else
        LOGE( TAG, "can not use this function with CAM mode", "" );

    return false;
}
#endif

bool CVideoStreamer::readAfter(cv::OutputArray output, int addFrameId)
{
#ifdef DEBUG_MODE
    if(m_selection > (int)nsCmn::FILE_TYPE_CAM)
    {
        if( seekFrame( getFrameNumber() + addFrameId ) == true )
            return read(output);
        else
            return false;
    }
#endif
    cv::Mat frame;
    for(int count = 0; count < addFrameId-1; ++count)
    {
        if(read(frame) == false)
            return false;
    }

    return read(output);
}

bool CVideoStreamer::seekFrame(time_t tsms)
{
    bool isSuccess = false;
    int64_t frame;

    frame = av_rescale( static_cast<int64_t>(tsms), m_pFormatCtx->streams[m_videoStream]->time_base.den, m_pFormatCtx->streams[m_videoStream]->time_base.num);
    frame/=1000;

    if(avformat_seek_file( m_pFormatCtx, m_videoStream, 0, frame, frame, AVSEEK_FLAG_FRAME) < 0)
        return isSuccess;

    avcodec_flush_buffers( m_pCodecCtx );
    isSuccess = true;

    return isSuccess;
}

bool CVideoStreamer::seekFrame(int frameId)
{
    bool isSuccess = false;
    int64_t frame;

    double fps = getFps();

    int64_t tsms = frameId * (1000/fps);

    frame = av_rescale( tsms, m_pFormatCtx->streams[m_videoStream]->time_base.den, m_pFormatCtx->streams[m_videoStream]->time_base.num);
    frame/=1000;

    if(avformat_seek_file( m_pFormatCtx, m_videoStream, 0, frame, frame, AVSEEK_FLAG_FRAME) < 0)
        return isSuccess;

    avcodec_flush_buffers( m_pCodecCtx );
    isSuccess = true;

    return isSuccess;
}

bool CVideoStreamer::setFrameInfo()
{
    bool isSuccess = false;

    cv::Mat frame;

    isSuccess = read(frame);

    if( isSuccess == true )
    {
        m_fullWidth = frame.cols;
        m_fullHeight = frame.rows;
        m_type = frame.type();
    }

    LOGI( TAG, "Set video capture is finished, Result: %s fullWidth: %f, fullHeight: %f, fps: %f"
    , nsCmn::boolToText(isSuccess), m_fullWidth , m_fullHeight, getFps() );

    return isSuccess;
}

double CVideoStreamer::getFps()
{
    return av_q2d(m_pFormatCtx->streams[m_videoStream]->r_frame_rate);
//  return 0;
}

int CVideoStreamer::getFrameNumber()
{
    return  m_frameNumber;//m_pFrame->coded_picture_number;
//  return 0;
}

double CVideoStreamer::getMsTimeStamp()
{
    return (getFrameNumber() / getFps())*1000;
//  return 0;
}

//void CVideoStreamer::fill_bitmap(AndroidBitmapInfo*  info, void *pixels, AVFrame *pFrame)
//{
//  uint8_t *frameLine;
//
//  int  yy;
//  for (yy = 0; yy < info->height; yy++)
//  {
//      uint8_t*  line = (uint8_t*)pixels;
//      frameLine = (uint8_t *)pFrame->data[0] + (yy * pFrame->linesize[0]);
//
//      int xx;
//      for (xx = 0; xx < info->width; xx++)
//      {
//          int out_offset = xx * 4;
//          int in_offset = xx * 3;
//
//          line[out_offset] = frameLine[in_offset];
//          line[out_offset+1] = frameLine[in_offset+1];
//          line[out_offset+2] = frameLine[in_offset+2];
//          line[out_offset+3] = 0;
//      }
//      pixels = (char*)pixels + info->stride;
//  }
//}
//
//void CVideoStreamer::drawFrame(std::string bitmap)
//{
//  AndroidBitmapInfo  info;
//  void*              pixels;
//  int                ret;
//
//  int err;
//  int i;
//  int frameFinished = 0;
//  AVPacket packet;
//  static struct SwsContext *img_convert_ctx;
//  int64_t seek_target;
//
//  if ((ret = AndroidBitmap_getInfo(m_env, bitmap, &info)) < 0)
//  {
//      LOGE( TAG, "AndroidBitmap_getInfo() failed ! error=%d", ret);
//      return;
//  }
//
//  LOGE( TAG,"Checked on the bitmap" );
//
//  if ((ret = AndroidBitmap_lockPixels(m_env, bitmap, &pixels)) < 0)
//      LOGE(TAG, "AndroidBitmap_lockPixels() failed ! error=%d", ret);
//
//  LOGE( TAG, "Grabbed the pixels" );
//
//  i = 0;
//  while((i==0) && (av_read_frame(pFormatCtx, &packet)>=0))
//  {
//      if(packet.stream_index==videoStream)
//      {
//          avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
//
//          if(frameFinished)
//          {
//              LOGE(TAG, "packet pts %llu", packet.pts);
//              // This is much different than the tutorial, sws_scale
//              // replaces img_convert, but it's not a complete drop in.
//              // This version keeps the image the same size but swaps to
//              // RGB24 format, which works perfect for PPM output.
//              int target_width = 320;
//              int target_height = 240;
//              img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
//                                               pCodecCtx->pix_fmt,
//                                               target_width, target_height, PIX_FMT_RGB24, SWS_BICUBIC,
//                                               NULL, NULL, NULL);
//              if(img_convert_ctx == NULL)
//              {
//                  LOGE(TAG, "could not initialize conversion context" );
//                  return;
//              }
//              sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
//
//              // save_frame(pFrameRGB, target_width, target_height, i);
//              fill_bitmap(&info, pixels, pFrameRGB);
//              i = 1;
//          }
//      }
//      av_free_packet(&packet);
//  }
//
//  AndroidBitmap_unlockPixels(env, bitmap);
//}
//
//void CVideoStreamer::drawFrameAt(JNIEnv * env, jobject this, jstring bitmap, jint secs)
//{
//AndroidBitmapInfo  info;
//void*              pixels;
//int                ret;
//
//int err;
//int i;
//int frameFinished = 0;
//AVPacket packet;
//static struct SwsContext *img_convert_ctx;
//int64_t seek_target;
//
//if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
//LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
//return;
//}
//LOGE("Checked on the bitmap");
//
//if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
//LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
//}
//LOGE("Grabbed the pixels");
//
//seek_frame(secs * 1000);
//
//i = 0;
//while ((i== 0) && (av_read_frame(pFormatCtx, &packet)>=0)) {
//if(packet.stream_index==videoStream) {
//avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
//
//if(frameFinished) {
//// This is much different than the tutorial, sws_scale
//// replaces img_convert, but it's not a complete drop in.
//// This version keeps the image the same size but swaps to
//// RGB24 format, which works perfect for PPM output.
//int target_width = 320;
//int target_height = 240;
//img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
//                               pCodecCtx->pix_fmt,
//                               target_width, target_height, PIX_FMT_RGB24, SWS_BICUBIC,
//                               NULL, NULL, NULL);
//if(img_convert_ctx == NULL) {
//LOGE("could not initialize conversion context\n");
//return;
//}
//sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
//
//// save_frame(pFrameRGB, target_width, target_height, i);
//fill_bitmap(&info, pixels, pFrameRGB);
//i = 1;
//}
//}
//av_free_packet(&packet);
//}
//
//AndroidBitmap_unlockPixels(env, bitmap);
//}