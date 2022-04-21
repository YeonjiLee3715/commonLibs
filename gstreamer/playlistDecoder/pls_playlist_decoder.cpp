#include "pls_playlist_decoder.hpp"

#include "../utils/CStrManager.h"

PLSPlaylistDecoder::PLSPlaylistDecoder()
{

}

PLSPlaylistDecoder::~PLSPlaylistDecoder()
{

}

bool PLSPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;

    if( nsCmn::compare(content_type.c_str(), content_type.size(), "audio/x-scpls")
            || nsCmn::compare(content_type.c_str(), content_type.size(), "application/pls+xml") )
        result = true;

    return result;
}

std::vector<std::string> PLSPlaylistDecoder::extract_media_streams(const std::string& data)
{
    std::vector<std::string> streams;

    std::istringstream iss(data);
    std::string line;

    while (std::getline(iss, line))
    {
        nsCmn::trim(line);
        if (!line.empty() and line.front() != '#')
        {
            auto eq_sign = std::find(std::begin(line), std::end(line), '=');
            if (eq_sign != std::end(line))
            {
                auto eq_sign_pos = std::distance(std::begin(line), eq_sign);
                auto k = line.substr(0, eq_sign_pos);
                auto v = line.substr(eq_sign_pos + 1, line.size() - eq_sign_pos - 1);
                if (strncasecmp("file", k.c_str(), 4) == 0)
                {
                    streams.push_back(v);
                }
            }
        }
    }

    return streams;
}

std::string PLSPlaylistDecoder::desc() const
{
    return std::string("PLS playlist decoder");
}
