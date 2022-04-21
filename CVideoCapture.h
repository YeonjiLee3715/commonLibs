#ifndef AROUNDVIEW_BASEVIDEOCAPTURE_H
#define AROUNDVIEW_BASEVIDEOCAPTURE_H

#include "cmnDef.h"

#include "cmnOpenCVDef.h"

class CVideoCapture : public cv::VideoCapture {
public:
    CVideoCapture();

    bool setVideoCapture();
    bool setVideoCapture(int nCamera);
    bool setVideoCapture(const std::string &filePath);
    bool setFrameInfo();

#ifdef DEBUG_MODE

    bool readAt(cv::OutputArray output, int frameId);
    bool readAfter(cv::OutputArray output, time_t msec);
#endif //DEBUG_MODE
    bool readAfter(cv::OutputArray output, int addFrameId);

    void setMultiplier(int multiplier);
    bool isInitComplete() { return m_isInitComplete; };

    double getFullWidth() { return m_fullWidth; };
    double getFullHeight() { return m_fullHeight; };
    int getType() { return m_type; };

private:

#ifdef DEBUG_MODE
    int m_selection;
#endif //DEBUG_MODE

    double m_fullWidth;
    double m_fullHeight;
    int m_type;

    bool m_isInitComplete;

    int m_multiplier;
};

#endif // AROUNDVIEW_BASEVIDEOCAPTURE_H