#ifndef MSGTHREAD_H
#define MSGTHREAD_H

#include <QThread>

class MsgThread : public QThread
{
    Q_OBJECT
public:
    MsgThread(QObject *parent = 0);
    void run();
	~MsgThread();
signals:

public slots:

};

#endif // MSGTHREAD_H
