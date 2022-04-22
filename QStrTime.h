#ifndef QSTRTIME_H
#define QSTRTIME_H

#include <stdio.h>
#include <QString>

using namespace std;

class QStrTime
{
public:
    QStrTime();

    static QString ConvertIntTimeToString(int sec);
    static QString ConvertIntTimeToStringQTimeVer(int sec);

};


#endif // QSTRTIME_H
