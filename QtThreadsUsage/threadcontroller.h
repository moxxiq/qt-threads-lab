#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QMutex>
#include "hashthreadedworker.h"

const QColor colorSet[] =
{
    QColor(255, 0, 0),
    QColor(255, 127, 0),
    QColor(255, 255, 0),
    QColor(127, 255, 0),
    QColor(0, 255, 0),
    QColor(0, 255, 127),
    QColor(0, 255, 255),
    QColor(0, 127, 255),
    QColor(0, 0, 255),
    QColor(127, 0, 255),
    QColor(255, 0, 255),
    QColor(255, 0, 127),
};
const int colorQuantity = *(&colorSet + 1) - colorSet;

class ThreadController : public QObject
{
    Q_OBJECT

private:
    int threads_counter;
    QVector<QThread*> threads;
    int sharedCurrentNumber;

protected:
    QMutex mutex;

public:
    explicit ThreadController(QObject *parent = nullptr);
    ~ThreadController();

    void add_task(HashThreadedWorker *task, QThread* thread);
    QString QStrFromPriority(enum QThread::Priority);

public slots:
    void isAllStopped();

signals:
    void finished();
};

#endif // THREADCONTROLLER_H
