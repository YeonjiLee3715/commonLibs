#include "m3u_playlist_decoder.hpp"

#include "../utils/CStrManager.h"

M3UPlaylistDecoder::M3UPlaylistDecoder()
{

}

M3UPlaylistDecoder::~M3UPlaylistDecoder()
{

}

bool M3UPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;

    if( nsCmn::compare(content_type.c_str(), content_type.size(), "audio/mpegurl", false )
            || nsCmn::compare(content_type.c_str(), content_type.size(), "audio/x-mpegurl", false ) )
        result = true;

    return result;
}

std::vector<std::string> M3UPlaylistDecoder::extract_media_streams(const std::string& data)
{
    std::vector<std::string> streams;

    std::istringstream iss(data);
    std::string line;

    while (std::getline(iss, line))
    {
        nsCmn::trim(line);
        if (!line.empty() and line.front() != '#')
        {
            streams.push_back(line);
        }
    }

    return streams;
}

std::string M3UPlaylistDecoder::desc() const
{
    return std::string("M3U playlist decoder");
}
