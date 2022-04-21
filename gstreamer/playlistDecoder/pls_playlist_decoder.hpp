#ifndef __PLS_PLAYLIST_DECODER_HPP_INCLUDED__
#define __PLS_PLAYLIST_DECODER_HPP_INCLUDED__

#include <sstream>

#include <vector>

class PLSPlaylistDecoder
{
public:
    PLSPlaylistDecoder();
    ~PLSPlaylistDecoder();

    bool is_valid(const std::string& content_type) const;
    std::vector<std::string> extract_media_streams(const std::string& data);
    std::string desc() const;

private:
};

#endif
