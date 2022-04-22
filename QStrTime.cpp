#include "QStrTime.h"
#include <QTime>

QStrTime::QStrTime()
{

}

QString QStrTime::ConvertIntTimeToString(int sec)
{
    int convertTime = sec / 1000;

    int nHour = convertTime / 3600;
    int nRemainSec = convertTime % 3600;

    QString strHour;
    QString strMin = QString::fromStdString(to_string(nRemainSec / 60));
    QString strSec = QString::fromStdString(to_string(nRemainSec % 60));
    QString strCurrentTime;

    if(nHour > 0)
    {
        strHour = QString::fromStdString(to_string(nHour));
        strCurrentTime.append(strHour);
        strCurrentTime.append(":");
    }

    if( strMin.length() == 1)
        strCurrentTime.append("0");

    strCurrentTime.append(strMin);
    strCurrentTime.append(":");
    if(strSec.length() == 1)
       strCurrentTime.append("0");
    strCurrentTime.append(strSec);

    return strCurrentTime;
}

QString QStrTime::ConvertIntTimeToStringQTimeVer(int sec)
{
    int convertTime = sec / 1000;

    QString strCurrentTime = QDateTime::fromTime_t(convertTime).toUTC().toString("hh:mm:ss");

    return strCurrentTime;
}
