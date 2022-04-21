#ifndef __RAM_PLAYLIST_DECODER_HPP_INCLUDED__
#define __RAM_PLAYLIST_DECODER_HPP_INCLUDED__

#include <iomanip>
#include <sstream>

#include <vector>

class RAMPlaylistDecoder
{
public:
    RAMPlaylistDecoder();
    ~RAMPlaylistDecoder();

    bool is_valid(const std::string& content_type) const;
    std::vector<std::string> extract_media_streams(const std::string& data);
    std::string desc() const;

private:
};

#endif
