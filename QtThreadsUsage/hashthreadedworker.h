#ifndef HASHTHREADEDWORKER_H
#define HASHTHREADEDWORKER_H

#include <QObject>
#include <QThread>
#include <QColor>
#include <QDebug>
#include <QMutex>
#include <QAbstractEventDispatcher>
#include "sha256.h"

class HashThreadedWorker : public QObject
{
    Q_OBJECT

public:
    HashThreadedWorker();
    ~HashThreadedWorker();

    void setChunkSize(int size);
    void setHash(const QString hash_needed);
    void setColor(QColor);
    void setNumber(int currentThreadNumber);
    void setCurrentI(int * i);
    int getNumber();
    void setMutex(QMutex *sharedMutex);
    void setIsOrdered(bool state);
    void setStartPriority(QThread::Priority priority);
    QThread::Priority startPriority();

signals:
    void calc_step(int threadNumber, int current_i, QString currentHash, QColor lineColor);
    void gotAnswer(int number, QString hash);
    void finished();

public slots:
    void run();
    void pause();
    void resume();
    void cancel();

protected:
    enum State { IDLE, RUNNING, PAUSED};
    State state;
    bool isCancelled();
    QMutex* mutex;

private:
    QString hash_needed;
    QColor lineColor;
    int number;
    int chunkSize;
    // pointer to shared variable
    int* current_i;
    bool isOrdered;
    void doHardCalc(int thisThreadIter, int endIter, QString& hash);
    QThread::Priority startThreadPriority;
};




#endif // HASHTHREADEDWORKER_H
