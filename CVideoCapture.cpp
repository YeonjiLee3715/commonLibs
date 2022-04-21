#include "CVideoCapture.h"

#ifdef DEBUG_MODE
#include "cmnTypeDef.h"
#endif

#include "CLogger.h"
#include "CStrManager.h"

namespace nsCVideoCapture {
    const char *const TAG = "CVideoCapture";
}
using namespace nsCVideoCapture;

CVideoCapture::CVideoCapture()
: m_multiplier(0), m_fullWidth(0), m_fullHeight(0), m_isInitComplete(0), m_type(0)
{
#ifdef DEBUG_MODE
    m_selection = (int)nsCmn::FILE_TYPE_CAM;
#endif
}

void CVideoCapture::setMultiplier(int multiplier)
{
    m_multiplier = multiplier;
}

#if defined( OPENCV_3_2_0 )
#ifdef DEBUG_MODE
bool CVideoCapture::readAt(cv::OutputArray output, int frameId)
{
    if(m_selection > (int)nsCmn::FILE_TYPE_CAM)
    {
        if( set(cv::CAP_PROP_POS_FRAMES, (double)frameId) == true )
            return read(output);
        else
            return false;
    }
    else
        LOGE( TAG, "can not use this function with CAM mode", "" );
    
    return false;
}

bool CVideoCapture::readAfter(cv::OutputArray output, time_t msec)
{
    if(m_selection > (int)nsCmn::FILE_TYPE_CAM)
    {
        double frameMsec = get(cv::CAP_PROP_POS_MSEC);
        if( set(cv::CAP_PROP_POS_MSEC, frameMsec + (double)msec) == true )
            return read(output);
        else
            return false;
    }
    else
        LOGE( TAG, "can not use this function with CAM mode", "" );
    
    return false;
}
#endif

bool CVideoCapture::readAfter(cv::OutputArray output, int addFrameId)
{
#ifdef DEBUG_MODE
    if(m_selection > (int)nsCmn::FILE_TYPE_CAM)
    {
        double frameId = get(cv::CAP_PROP_POS_FRAMES);
        if( set(cv::CAP_PROP_POS_FRAMES, (frameId+(double)addFrameId)) == true )
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

#endif // ifdef( OPENCV_VERSION )

bool CVideoCapture::setVideoCapture( int nCamera )
{
    bool isSuccess = false;

#ifdef DEBUG_MODE
    m_selection = (int)nsCmn::FILE_TYPE_CAM;
#endif

    do
    {
        isSuccess = open(nCamera);

        if( isSuccess == false )
        {
            LOGE( TAG, "Falied open camera device. camera number: %d", nCamera);
            break;
        }

        isSuccess = setFrameInfo();

    }while(false);

    m_isInitComplete = isSuccess;

    return isSuccess;
}

bool CVideoCapture::setVideoCapture( const std::string& filePath )
{
    bool isSuccess = false;

    do
    {
#ifdef DEBUG_MODE
        int pos = filePath.find_last_of('.');
        if( pos == std::string::npos )
        {
            LOGE( TAG, "Wrong file. filePath: %s", filePath.c_str());
            break;
        }

        ++pos; //skip '.'

        std::string strExtension = filePath.substr( pos, (filePath.size()-pos)-1);

        if( nsCmn::compare(strExtension.c_str(), "mp4", false) )
            m_selection = (int)nsCmn::FILE_TYPE_MP4;
        else if( nsCmn::compare(strExtension.c_str(), "avi", false) )
            m_selection = (int)nsCmn::FILE_TYPE_AVI;
#endif
        isSuccess = open(filePath);

        if( isSuccess == false )
        {
            LOGE( TAG, "Falied to open sample file. path: %s", filePath.c_str());
            break;
        }

        isSuccess = setFrameInfo();

    }while(false);

    m_isInitComplete = isSuccess;

    return isSuccess;
}

bool CVideoCapture::setVideoCapture()
{
    bool isSuccess = false;
        
#ifdef DEBUG_MODE
    std::string cur_path = "./sample/sample.";
    
    m_selection = (int)nsCmn::FILE_TYPE_CAM;

    std::system("clear");
    std::cout << "select sample file type:"<<std::endl;
    std::cout << "1. cam (device) " << std::endl;
    std::cout << "2. mp4" << std::endl;
    std::cout << "3. avi" << std::endl;
    std::cin >> m_selection;
  
    switch(m_selection)
    {
    case nsCmn::FILE_TYPE_CAM:
        break;
    case nsCmn::FILE_TYPE_MP4:
        cur_path.append("mp4");
        break;
    case nsCmn::FILE_TYPE_AVI:
        cur_path.append("avi");
        break;
    default:
        cur_path.append("mp4");
        m_selection = (int)nsCmn::FILE_TYPE_MP4;
        break;
    }

    switch(m_selection)
    {
    case nsCmn::FILE_TYPE_CAM:
        isSuccess = open(0);
        break;
    case nsCmn::FILE_TYPE_MP4:
    case nsCmn::FILE_TYPE_AVI:
        isSuccess = open(cur_path);
        break;
    default:
        break;
    }

    if( isSuccess == false )
    {
        if(m_selection == nsCmn::FILE_TYPE_CAM)
            LOGE( TAG, "Falied to open camera device. camera number: %d", 0);
        else
            LOGE( TAG, "Falied to open sample file. path: %s",  cur_path.c_str());
    }
    
#else

    isSuccess = open(0);
    
    if(isSuccess == false)
        LOGE( TAG, "Falied open camera device" );
       
#endif
    
    if(isSuccess == true)
        isSuccess = setFrameInfo();

    m_isInitComplete = isSuccess;
    
    return isSuccess;
}

bool CVideoCapture::setFrameInfo()
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

    LOGI( TAG, "Set video capture is success, fullWidth: %f, fullHeight: %f, fps: %f", m_fullWidth , m_fullHeight, get(CV_CAP_PROP_FPS) );

    return isSuccess;
}

