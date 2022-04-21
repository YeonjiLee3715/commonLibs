#ifndef CPLAYLISTDECODER_H
#define CPLAYLISTDECODER_H

#include <string>

#include "../audioPlayer/playlistDecoder/asx_playlist_decoder.hpp"
#include "../audioPlayer/playlistDecoder/m3u_playlist_decoder.hpp"
#include "../audioPlayer/playlistDecoder/pls_playlist_decoder.hpp"
#include "../audioPlayer/playlistDecoder/ram_playlist_decoder.hpp"
#include "../audioPlayer/playlistDecoder/xspf_playlist_decoder.hpp"

class CPlayListDecoder
{

public:
    enum ePLAYLIST_TYPE{
        PLAYLIST_TYPE_NONE,
        PLAYLIST_TYPE_PLS,
        PLAYLIST_TYPE_M3U,
        PLAYLIST_TYPE_ASX,
        PLAYLIST_TYPE_RAM,
        PLAYLIST_TYPE_XSPF
    };

    CPlayListDecoder();
    ~CPlayListDecoder();

    ePLAYLIST_TYPE getPlayListType( const std::string& contentType );
    std::vector<std::string> extract_media_streams( ePLAYLIST_TYPE contentType, const std::string& data);

    ASXPlaylistDecoder* m_asxDecoder;
    M3UPlaylistDecoder* m_m3uDecoder;
    PLSPlaylistDecoder* m_plsDecoder;
    RAMPlaylistDecoder* m_ramDecoder;
    XSPFPlaylistDecoder* m_xspfDecoder;
};

#endif // CPLAYLISTDECODER_H
