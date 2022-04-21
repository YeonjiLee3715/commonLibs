//
// Created by 이연지 on 2017-06-01.
//

#ifndef AROUNDVIEW_CBASECLASS_H
#define AROUNDVIEW_CBASECLASS_H


class CBaseClass
{
public:
    CBaseClass();
    virtual ~CBaseClass(){};

    virtual bool init(){ return true; };

    void setStatus( bool isSet ){ m_isSet = isSet; };
    bool IsSet(){ return m_isSet; };

private:
    bool m_isSet;
};


#endif //AROUNDVIEW_CBASECLASS_H
