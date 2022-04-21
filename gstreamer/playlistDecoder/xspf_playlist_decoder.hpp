#ifndef __XSPF_PLAYLIST_DECODER_HPP_INCLUDED__
#define __XSPF_PLAYLIST_DECODER_HPP_INCLUDED__

#include <iomanip>
#include <sstream>
#include <vector>

#include "pugixml/pugixml.hpp"

class XSPFPlaylistDecoder
{
public:
    XSPFPlaylistDecoder();
    ~XSPFPlaylistDecoder();

    bool is_valid(const std::string& content_type) const;
    std::vector<std::string> extract_media_streams(const std::string& data);
    std::string desc() const;

private:
    pugi::xml_document playlist_doc;
};

#endif
