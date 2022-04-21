//
// Created by 이연지 on 2017-07-06.
//
#include "CRapidxmlHelper.h"

#if defined( __linux__ ) || defined( __APPLE__ )
#include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include "CStrManager.h"
#include "CLogger.h"

namespace nsCRapidxmlHelper{
    const char* TAG = "CRapidxmlHelper";
}

using namespace nsCRapidxmlHelper;

CRapidxmlHelper::CRapidxmlHelper( unsigned char* buff )
        : m_doc(NULL), m_curNode(NULL), m_rootNode(NULL), m_curAttribute(NULL), m_isSet(false)
{
    try
    {
        if(buff == NULL)
        {
            rapidxml::parse_error err("empty data", NULL);
            throw(err);
        }

        m_doc = new rapidxml::xml_document<char>;
        if( m_doc == NULL )
        {
            rapidxml::parse_error err("make xml document falied", NULL);
            throw(err);
        }

        m_doc->parse<0>((char*)buff);

        if( m_doc != NULL )
        {
            m_rootNode = m_doc->first_node();

            clearCurrentNode( );
            m_isSet = true;
        }
        else
            m_isSet = false;

    } catch(rapidxml::parse_error err)
    {
        LOGE(TAG, "Failed set xml parser, %s", err.what());
    }
}

CRapidxmlHelper::~CRapidxmlHelper()
{
    clear();
}

unsigned char* CRapidxmlHelper::getCurrentNodeName( )
{
    if( IsValid(NODE) == false )
        return NULL;

    return (unsigned char*)m_curNode->name();
}

unsigned char* CRapidxmlHelper::getCurrentNodeData( )
{
    if( IsValid(NODE) == false )
        return NULL;

    return (unsigned char*)m_curNode->value();
}

unsigned char* CRapidxmlHelper::getCurrentAttributeName( )
{
    if( IsValid(ATTRIBUTE) == false )
        return NULL;

    return (unsigned char*)m_curAttribute->name();
}

unsigned char* CRapidxmlHelper::getCurrentAttributeData( )
{
    if( IsValid(ATTRIBUTE) == false )
        return NULL;

    return (unsigned char*)m_curAttribute->value();
}

rapidxml::xml_node<char>* CRapidxmlHelper::setCurrentNode( rapidxml::xml_node<char>* node )
{
    if( m_doc == NULL || node == NULL )
    {
        LOGE(TAG, "Can't set the node. m_doc is NULL: %s, node is NULL: %s"
        , (m_doc == NULL ? "true" : "false")
        , (node == NULL ? "true" : "false"));
        return NULL;
    }

    m_curNode = node;
    m_curAttribute = firstAttribute( );

    return m_curNode;
}

rapidxml::xml_node<char>* CRapidxmlHelper::getNodeByName( rapidxml::xml_node<char>* node, const char* name, size_t name_size,  bool case_sensitive )
{
    if( IsValid(DOC) == false || node == NULL || name == NULL )
        return NULL;

    if(name_size <= 0)
    {
        name_size = rapidxml::internal::measure( name );
        if( name_size <= 0 )
            return NULL;
    }

    rapidxml::xml_node<char>* result = NULL;

    while( node != NULL )
    {
        if( rapidxml::internal::compare(node->name(), node->name_size(), name, name_size, case_sensitive) == true )
        {
            result = node;
            break;
        }
        else
        {
            rapidxml::xml_node<char>* child = node->first_node();
            if(child != NULL)
                result = getNodeByName(child, name, name_size, case_sensitive);

            if( result != NULL )
                break;

            node = node->next_sibling();
        }
#if defined( __linux__ ) || defined( __APPLE__ )
        usleep( 3 * 1000 );
#elif defined(_WIN32) || defined(_WIN64)
        Sleep( 3 );
#endif
    }

    return  result;
}

rapidxml::xml_node<char>* CRapidxmlHelper::getNodeByPath(rapidxml::xml_node<char>* node, char* path, std::size_t path_size, bool case_sensitive)
{
    if( IsValid(DOC) == false || node == nullptr || path == nullptr )
        return nullptr;

    if(path_size <= 0)
    {
        path_size = rapidxml::internal::measure( path );
        if( path_size <= 0 )
            return nullptr;
    }

    std::vector<std::string> vecPath = nsCmn::split( nsCmn::toString(path, path_size), "/", true, false );
    if( vecPath.empty() )
    {
        LOGE( TAG, "Wrong path. %s", nsCmn::toString( path, path_size ).c_str() );
        return nullptr;
    }

    rapidxml::xml_node<char>* result = node;

    std::vector<std::string>::const_iterator it = vecPath.cbegin();
    if( nsCmn::compare( result->name(), result->name_size(), it->c_str(), it->size(), false ) )
        ++it;

    while( it != vecPath.cend() )
    {
        result = result->first_node( it->c_str(), it->size(), false );

        if( result == nullptr )
            break;

        ++it;

#if defined( __linux__ ) || defined( __APPLE__ )
        usleep( 3 * 1000 );
#elif defined(_WIN32) || defined(_WIN64)
        Sleep( 3 );
#endif
    }

    return  result;
}

rapidxml::xml_attribute<char>* CRapidxmlHelper::getAttributeByPath( rapidxml::xml_node<char>* node, char * path, std::size_t path_size, bool case_sensitive )
{
    if( IsValid( DOC ) == false || node == nullptr || path == nullptr )
        return nullptr;

    if( path_size <= 0 )
    {
        path_size = rapidxml::internal::measure( path );
        if( path_size <= 0 )
            return nullptr;
    }

    int posAt = nsCmn::findLastIndexOf( (const char*)path, path_size, "@" );
    int posSlash = nsCmn::findLastIndexOf( (const char*)path, path_size, "/" );

    if( ( posAt < 0 && posSlash >= 0 ) || ( posSlash >= 0 && posAt >= posSlash ) )
    {
        LOGE( TAG, "Wrong path. %s", nsCmn::toString( path, path_size ).c_str() );
        return nullptr;
    }

    std::vector<std::string> vecSymbols;
    vecSymbols.push_back( "/" );
    vecSymbols.push_back( "@" );

    std::vector<std::string> vecPath = nsCmn::split( nsCmn::toString( path, path_size ), vecSymbols, true, false );

    if( vecPath.empty() )
    {
        LOGE( TAG, "Wrong path. %s", nsCmn::toString( path, path_size ).c_str() );
        return nullptr;
    }

    std::string attributeName = vecPath.back();
    vecPath.resize( vecPath.size()-1 );

    rapidxml::xml_node<char>* resNode = node;

    if( vecPath.empty() == false )
    {
        std::vector<std::string>::const_iterator it = vecPath.cbegin();
        if( nsCmn::compare( resNode->name(), resNode->name_size(), it->c_str(), it->size(), false ) )
            ++it;

        while( it != vecPath.cend() )
        {
            resNode = resNode->first_node( it->c_str(), it->size(), false );

            if( resNode == nullptr )
                break;

            ++it;

#if defined( __linux__ ) || defined( __APPLE__ )
            usleep( 3 * 1000 );
#elif defined(_WIN32) || defined(_WIN64)
            Sleep( 3 );
#endif
        }
    }

    if( resNode == nullptr )
        return nullptr;

    return resNode->first_attribute( attributeName.c_str(), attributeName.size(), false );
}

bool CRapidxmlHelper::IsValid( eCHECK_RANGE range )
{
    bool isValid = false;

    switch(range)
    {
        case ATTRIBUTE:
            if(m_curAttribute == NULL)
            {
                LOGE(TAG, "Node access faild. m_curAttribute is NULL", "");
                break;
            }
        case NODE:
            if(m_curNode == NULL)
            {
                LOGE(TAG, "Node access faild. m_curNode is NULL", "");
                break;
            }
        case DOC:
            if(m_doc == NULL)
            {
                LOGE(TAG, "Node access faild. m_doc is NULL", "");
                break;
            }

            isValid = true;
    }

    return isValid;
}

rapidxml::xml_attribute<char>* CRapidxmlHelper::firstAttribute(const char* name, std::size_t name_size, bool case_sensitive)
{
    if( IsValid(NODE) == false )
        return NULL;

    m_curAttribute = (rapidxml::xml_attribute<char>*)m_curNode->first_attribute(name, name_size, case_sensitive);

    return m_curAttribute;
}

rapidxml::xml_attribute<char>* CRapidxmlHelper::previousAttribute(const char* name, std::size_t name_size, bool case_sensitive)
{
    if( IsValid(ATTRIBUTE) == false )
        return NULL;

    m_curAttribute = (rapidxml::xml_attribute<char>*)m_curAttribute->previous_attribute(name, name_size, case_sensitive);

    return m_curAttribute;
}

rapidxml::xml_attribute<char>* CRapidxmlHelper::nextAttribute( const char* name, std::size_t name_size, bool case_sensitive )
{
    if( IsValid( ATTRIBUTE ) == false )
        return NULL;

    m_curAttribute = ( rapidxml::xml_attribute<char>* ) m_curAttribute->next_attribute( name, name_size, case_sensitive );

    return m_curAttribute;
}

void CRapidxmlHelper::clear()
{
    m_isSet = false;

    m_curNode = nullptr;
    m_curAttribute = nullptr;
    m_rootNode = nullptr;

    if( m_doc != nullptr )
    {
        m_doc->clear();
        delete m_doc;
        m_doc = nullptr;
    }
}

rapidxml::xml_node<char>* CRapidxmlHelper::parentNode( )
{
    if( IsValid(NODE) == false )
        return NULL;

    m_curNode = (rapidxml::xml_node<char>*)m_curNode->parent();
    m_curAttribute = firstAttribute( );

    return  m_curNode;
}

rapidxml::xml_node<char>* CRapidxmlHelper::firstChildNode(const char* name, std::size_t name_size, bool case_sensitive)
{
    if( IsValid(NODE) == false )
        return NULL;

    m_curNode = (rapidxml::xml_node<char>*)m_curNode->first_node(name, name_size, case_sensitive);
    m_curAttribute = firstAttribute( );

    return  m_curNode;
}

rapidxml::xml_node<char>* CRapidxmlHelper::previousSibiling(const char* name, std::size_t name_size, bool case_sensitive)
{
    if( IsValid(NODE) == false )
        return NULL;

    m_curNode = (rapidxml::xml_node<char>*)m_curNode->previous_sibling(name, name_size, case_sensitive);
    m_curAttribute = firstAttribute( );

    return  m_curNode;
}

rapidxml::xml_node<char>* CRapidxmlHelper::nextSibiling(const char* name, std::size_t name_size, bool case_sensitive)
{
    if( IsValid(NODE) == false )
        return NULL;

    m_curNode = (rapidxml::xml_node<char>*)m_curNode->next_sibling(name, name_size, case_sensitive);
    m_curAttribute = firstAttribute( );

    return  m_curNode;
}

rapidxml::xml_node<char>* CRapidxmlHelper::getRootNode( )
{
    return m_rootNode;
}

rapidxml::xml_node<char>* CRapidxmlHelper::getCurrentNode( )
{
    return m_curNode;
}

rapidxml::xml_node<char>* CRapidxmlHelper::clearCurrentNode( )
{
    if( IsValid(DOC) == false )
        return NULL;

    m_curNode = m_rootNode;
    m_curAttribute = firstAttribute();

    return  m_curNode;
}
