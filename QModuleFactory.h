#ifndef QMODULEFACTORY_H
#define QMODULEFACTORY_H

#include "QBaseModule.h"
#include "ModuleEnum.h"

class QModuleFactory
{
public:
    QModuleFactory();
    ~QModuleFactory();

    static QBaseModule* CreateModule( QModuleEnum::eMODULE nModuleId );
};

#endif // QMODULEFACTORY_H
