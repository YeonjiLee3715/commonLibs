#include "ram_playlist_decoder.hpp"

#include "../utils/CStrManager.h"

RAMPlaylistDecoder::RAMPlaylistDecoder()
{

}

RAMPlaylistDecoder::~RAMPlaylistDecoder()
{

}

bool RAMPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;

    if( nsCmn::compare(content_type.c_str(), content_type.size(), "audio/x-pn-realaudio", false )
        || nsCmn::compare(content_type.c_str(), content_type.size(), "audio/vnd.rn-realaudio", false ) )
    {
        result = true;
    }

    return result;
}

std::vector<std::string> RAMPlaylistDecoder::extract_media_streams(const std::string& data)
{
    std::vector<std::string> streams;

    std::istringstream iss(data);
    std::string line;

    while (std::getline(iss, line))
    {
        nsCmn::trim(line);
        if( line.empty()== false && line.front() != '#')
        {
            streams.push_back(line);
        }
    }

    return streams;
}

std::string RAMPlaylistDecoder::desc() const
{
    return std::string("RAM playlist decoder");
}
