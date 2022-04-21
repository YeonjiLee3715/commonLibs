#include "xspf_playlist_decoder.hpp"

#include "../logger/CLogger.h"
#include "../utils/CStrManager.h"

XSPFPlaylistDecoder::XSPFPlaylistDecoder()
{

}

XSPFPlaylistDecoder::~XSPFPlaylistDecoder()
{

}

bool XSPFPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;

    if( nsCmn::compare(content_type.c_str(), content_type.size(), "application/xspf+xml", false ) )
        result = true;

    return result;
}

std::vector<std::string> XSPFPlaylistDecoder::extract_media_streams(const std::string& data)
{
    std::vector<std::string> streams;

    pugi::xml_parse_result parsed = playlist_doc.load_buffer(data.c_str(), data.size());
    if (parsed)
    {
        try
        {
            pugi::xpath_node_set nodes = playlist_doc.select_nodes("//track/location");
            for (auto& node : nodes)
            {
                if (not node.node().text().empty()) {
                    streams.push_back(node.node().text().as_string());
                }
            }
        } catch (pugi::xpath_exception& exc)
        {
            QLOGE( QString( "Parsing XSPF playlist failed: %1" ).arg( exc.what() ) );
        }
    }
    else
    {
        QLOGE( QString( "Parsing XSPF playlist failed: %1" ).arg( parsed.description() ) );
    }

    return streams;
}

std::string XSPFPlaylistDecoder::desc() const
{
    return std::string("XSPF playlist decoder");
}
