#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);
    ~Worker(void);
    
signals:
    void finished();
    void error(QString err);
    
public slots:
    void process();
    
private:


};

#endif // WORKER_H
