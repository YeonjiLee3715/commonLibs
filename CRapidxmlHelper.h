//
// Created by 이연지 on 2017-07-06.
//

#ifndef _RAPIDXMLHELPER_H
#define _RAPIDXMLHELPER_H

#include "cmnDef.h"

#include "rapidxml-1.13/rapidxml.hpp"
//rapidxml helper class. rapidxml은 utf-8, utf-16만 지원.

class CRapidxmlHelper
{
private:
    typedef enum eCHECK_RANGE {
        DOC = 0,
        NODE,
        ATTRIBUTE
    }eCHECK_RANGE;

public:
    CRapidxmlHelper( unsigned char* buff );
    ~CRapidxmlHelper();

    bool isSet(){ return m_isSet; };

    unsigned char* getCurrentNodeData();
    unsigned char* getCurrentNodeName();

    unsigned char* getCurrentAttributeData();
    unsigned char* getCurrentAttributeName();

    rapidxml::xml_node<char>* setCurrentNode(rapidxml::xml_node<char>* node); //< m_curNode가 node로 변경됩니다. m_curAttribute가 node의 firstAttribute로 변경됩니다.

    rapidxml::xml_node<char>* getRootNode();
    rapidxml::xml_node<char>* getCurrentNode();

    rapidxml::xml_node<char>* clearCurrentNode();       //< m_curNode가 rootNode로 변경됩니다. m_curAttribute가 rootNode의 firstAttribute로 변경됩니다.

/*
* name : 이름을 통한 검색을 원할 때 설정 설정하지 않으면 각 함수에 따른 위치 반환. 예) previousSibiling() 하나 앞 노드 반환
* name_size : 이름을 통한 검색에서 name 문자열 길이. 0을 넣으면 알아서 계산.
* case_sensitive : 대소문자 구분여부
 * 원문 : \rapidxml-1.13\manual.html
*/

    rapidxml::xml_node<char>* getNodeByName(rapidxml::xml_node<char>* node, const char* name, std::size_t name_size=0, bool case_sensitive=true);
    rapidxml::xml_node<char>* getNodeByPath(rapidxml::xml_node<char>* node, char* path, std::size_t path_size=0, bool case_sensitive=true);

    rapidxml::xml_attribute<char>* getAttributeByPath( rapidxml::xml_node<char>* node, char* path, std::size_t path_size = 0, bool case_sensitive = true );

    rapidxml::xml_node<char>* parentNode();             //< m_curNode가 parentNode로 변경됩니다. m_curAttribute가 parentNode의 firstAttribute로 변경됩니다.
    rapidxml::xml_node<char>* firstChildNode(const char* name=NULL, std::size_t name_size=0, bool case_sensitive=true);         //< m_curNode가 firstChildNode로 변경됩니다. m_curAttribute가 firstChildNode의 firstAttribute로 변경됩니다.
    rapidxml::xml_node<char>* previousSibiling(const char* name=NULL, std::size_t name_size=0, bool case_sensitive=true);       //< m_curNode가 previousNode로 변경됩니다. m_curAttribute가 previousNode의 firstAttribute로 변경됩니다.
    rapidxml::xml_node<char>* nextSibiling(const char* name=NULL, std::size_t name_size=0, bool case_sensitive=true);           //< m_curNode가 nextNode로 변경됩니다. m_curAttribute가 nextNode의 firstAttribute로 변경됩니다.

    rapidxml::xml_attribute<char>* firstAttribute(const char* name=NULL, std::size_t name_size=0, bool case_sensitive=true);    //< m_curNode의 firstAttribute가 반환됩니다. m_curAttribute가 firstAttribute로 변경됩니다.
    rapidxml::xml_attribute<char>* previousAttribute(const char* name=NULL, std::size_t name_size=0, bool case_sensitive=true); //< m_curAttribute가 previousAttribute로 변경됩니다.
    rapidxml::xml_attribute<char>* nextAttribute(const char* name=NULL, std::size_t name_size=0, bool case_sensitive=true);     //< m_curAttribute가 nextAttribute로 변경됩니다.

    void clear();

private:
    bool IsValid( eCHECK_RANGE range );

private:
    rapidxml::xml_document<char>* m_doc;
    rapidxml::xml_node<char>* m_curNode;
    rapidxml::xml_attribute<char>* m_curAttribute;

    rapidxml::xml_node<char>* m_rootNode;

    bool m_isSet;
};


#endif //_RAPIDXMLHELPER_H
