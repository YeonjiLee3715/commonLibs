//
// Created by 이연지 on 2017-06-01.
//

#ifndef AROUNDVIEW_CVIDEOSTREAMER_H
#define AROUNDVIEW_CVIDEOSTREAMER_H
#include "cmnDef.h"

#include <string>
#include "cmnTypeDef.h"
#include "CBaseClass.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif

#include <android/bitmap.h>
#include "cmnOpenCVDef.h"

class CVideoStreamer : public CBaseClass
{
public:

    CVideoStreamer();
    virtual ~CVideoStreamer(){};

    virtual bool init();

    bool openFile( const char* fileName );

#ifdef DEBUG_MODE
    bool readAt(cv::OutputArray output, int frameId);
    bool readAfter(cv::OutputArray output, time_t msec);
#endif

    bool readAfter(cv::OutputArray output, int addFrameId);
    bool read( cv::OutputArray output );

    void setMultiplier(int multiplier);

    double getFullWidth(){ return m_fullWidth; };
    double getFullHeight(){ return m_fullHeight; };
    int getType(){ return m_type; };

    double getFps();
    int getFrameNumber();
    double getMsTimeStamp();

private:
    bool seekFrame(time_t tsms);
    bool seekFrame(int frameId);
    bool setFrameInfo();

private:
#ifdef DEBUG_MODE
    int m_selection;
#endif

    double m_fullWidth;
    double m_fullHeight;
    int m_type;

    int m_multiplier;

    std::string m_filePath;
    nsCmn::eFileType m_fileType;

    AVFormatContext* m_pFormatCtx;
    AVCodecContext* m_pCodecCtx;
    AVFrame* m_pFrame;
    AVFrame* m_pFrameRGB;

    int m_videoStream;

    int m_frameNumber;
};


#endif //AROUNDVIEW_CVIDEOSTREAMER_H
