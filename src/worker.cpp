#include "worker.h"

Worker::Worker(QObject *parent) :
    QObject(parent)
{
}

Worker::~Worker()
{
}

void Worker::process()
{
    qDebug("Hello World!");
    emit finished();
}
