#ifndef CEVENTLOOP_H
#define CEVENTLOOP_H

#include <QEventLoop>
#include <QTimer>

class CEventLoop : public QEventLoop
{
    Q_OBJECT
public:
    explicit CEventLoop(QObject *parent = nullptr);
    ~CEventLoop();
    bool IsTimeout();

    int exec( int msTimeout = 1000*60, ProcessEventsFlags flags = AllEvents);

signals:

public slots:
    void Timeout();

public Q_SLOTS:
    void quit();

private:
    bool m_bIsTimeout;
    QTimer m_timer;
};

#endif // CEVENTLOOP_H
