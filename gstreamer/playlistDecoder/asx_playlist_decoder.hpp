#ifndef __ASX_PLAYLIST_DECODER_HPP_INCLUDED__
#define __ASX_PLAYLIST_DECODER_HPP_INCLUDED__

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <string>
#include <vector>

class ASXPlaylistDecoder
{
public:
    ASXPlaylistDecoder();
    ~ASXPlaylistDecoder();

    bool is_valid(const std::string& content_type) const;
    std::vector<std::string> extract_media_streams(const std::string& data);
    std::string desc() const;

private:
};

#endif
