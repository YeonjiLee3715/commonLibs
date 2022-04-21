#ifndef __M3U_PLAYLIST_DECODER_HPP_INCLUDED__
#define __M3U_PLAYLIST_DECODER_HPP_INCLUDED__

#include <iomanip>
#include <regex>
#include <sstream>

#include <vector>

class M3UPlaylistDecoder
{
public:
    M3UPlaylistDecoder();
    ~M3UPlaylistDecoder();

    bool is_valid(const std::string& content_type) const;
    std::vector<std::string> extract_media_streams(const std::string& data);
    std::string desc() const;

private:
};

#endif
