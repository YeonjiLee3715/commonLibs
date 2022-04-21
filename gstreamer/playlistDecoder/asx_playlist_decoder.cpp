#include "asx_playlist_decoder.hpp"

#include "../utils/CStrManager.h"

ASXPlaylistDecoder::ASXPlaylistDecoder()
{
    xmlInitParser();
    LIBXML_TEST_VERSION;
}

ASXPlaylistDecoder::~ASXPlaylistDecoder()
{
    xmlCleanupParser();
}

bool ASXPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;

    if (nsCmn::compare(content_type.c_str(), content_type.size(), "audio/x-ms-wax", false )
            || nsCmn::compare(content_type.c_str(), content_type.size(), "video/x-ms-wvx", false )
        || nsCmn::compare(content_type.c_str(), content_type.size(), "video/x-ms-asf", false )
            || nsCmn::compare(content_type.c_str(), content_type.size(), "video/x-ms-wmv", false ) )
    {
        result = true;
    }

    return result;
}

std::vector<std::string> ASXPlaylistDecoder::extract_media_streams(const std::string& data)
{
    std::vector<std::string> streams;

    xmlDocPtr doc;
    xmlXPathContextPtr xpath_ctx;
    xmlXPathObjectPtr xpath_obj;

    const unsigned char* xpath_expr = BAD_CAST("//ref/@href");

    doc = xmlRecoverMemory(data.c_str(), data.size());
    if (doc == nullptr)
    {
        return streams;
    }

    xpath_ctx = xmlXPathNewContext(doc);
    if (xpath_ctx == nullptr)
    {
        xmlFreeDoc(doc);
        return streams;
    }

    xpath_obj = xmlXPathEvalExpression(xpath_expr, xpath_ctx);
    if (xpath_obj == nullptr)
    {
        xmlXPathFreeContext(xpath_ctx);
        xmlFreeDoc(doc);
        return streams;
    }

    auto nodes = xpath_obj->nodesetval;
    auto size = (nodes) ? nodes->nodeNr : 0;

    for (decltype(size) i = 0; i < size; ++i)
    {
        auto cur = nodes->nodeTab[i];
        if (cur->type == XML_ATTRIBUTE_NODE)
        {
            std::string s = (char*)xmlNodeGetContent(cur);
            nsCmn::trim(s);
            streams.push_back(s);
        }
    }

    xmlXPathFreeObject(xpath_obj);
    xmlXPathFreeContext(xpath_ctx);
    xmlFreeDoc(doc);

    return streams;
}

std::string ASXPlaylistDecoder::desc() const
{
    return std::string("ASX playlist decoder");
}

