#include "CPlayListDecoder.h"

#include "../utils/CStrManager.h"
#include "../logger/CLogger.h"

CPlayListDecoder::CPlayListDecoder()
    : m_asxDecoder(NULL), m_m3uDecoder(NULL), m_plsDecoder(NULL)
    , m_ramDecoder(NULL), m_xspfDecoder(NULL)
{
    m_asxDecoder = new ASXPlaylistDecoder;
    m_m3uDecoder = new M3UPlaylistDecoder;
    m_plsDecoder = new PLSPlaylistDecoder;
    m_ramDecoder = new RAMPlaylistDecoder;
    m_xspfDecoder = new XSPFPlaylistDecoder;
}

CPlayListDecoder::~CPlayListDecoder()
{
    if( m_asxDecoder != NULL )
        delete m_asxDecoder;
    if( m_m3uDecoder != NULL )
        delete m_m3uDecoder;
    if( m_plsDecoder != NULL )
        delete m_plsDecoder;
    if( m_ramDecoder != NULL )
        delete m_ramDecoder;
    if( m_xspfDecoder != NULL )
        delete m_xspfDecoder;
}

CPlayListDecoder::ePLAYLIST_TYPE CPlayListDecoder::getPlayListType(const std::string &contentType )
{

    QLOGD( contentType.c_str() );

    if( m_plsDecoder->is_valid(contentType) )
        return PLAYLIST_TYPE_PLS;
    if( m_m3uDecoder->is_valid(contentType) )
        return PLAYLIST_TYPE_M3U;
    if( m_asxDecoder->is_valid(contentType) )
        return PLAYLIST_TYPE_ASX;
    if( m_ramDecoder->is_valid(contentType) )
        return PLAYLIST_TYPE_RAM;
    if( m_xspfDecoder->is_valid(contentType) )
        return PLAYLIST_TYPE_XSPF;

    return PLAYLIST_TYPE_NONE;
}

std::vector<std::string> CPlayListDecoder::extract_media_streams(ePLAYLIST_TYPE contentType, const std::string &data)
{
    std::vector<std::string> vecUrls;

    switch (contentType)
    {
    case PLAYLIST_TYPE_PLS:
        vecUrls = m_plsDecoder->extract_media_streams( data );
        break;
    case PLAYLIST_TYPE_M3U:
        vecUrls = m_m3uDecoder->extract_media_streams( data );
        break;
    case PLAYLIST_TYPE_ASX:
        vecUrls = m_asxDecoder->extract_media_streams( data );
        break;
    case PLAYLIST_TYPE_RAM:
        vecUrls = m_ramDecoder->extract_media_streams( data );
        break;
    case PLAYLIST_TYPE_XSPF:
        vecUrls = m_xspfDecoder->extract_media_streams( data );
        break;
    case PLAYLIST_TYPE_NONE:
    default:
        break;
    }

    return vecUrls;
}
