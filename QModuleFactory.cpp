#include "QModuleFactory.h"

#include <CLogger.h>

namespace nsQModuleFactory{
    const char* TAG = "QModuleFactory";
}

using namespace nsQModuleFactory;

QModuleFactory::QModuleFactory()
{

}

QModuleFactory::~QModuleFactory()
{

}

QBaseModule* QModuleFactory::CreateModule( QModuleEnum::eMODULE nModuleId )
{
    QBaseModule* pModule = nullptr;

    switch( nModuleId )
    {
    /* create modules */
    /* case QModuleEnum::eMODULE::TestModule:
        pModule = new TestModule;
        break; */
    default:
        LOGE( TAG, "Wrong module id: %d", (int)nModuleId );
    }

    return pModule;
}
