#include "hashthreadedworker.h"

HashThreadedWorker::HashThreadedWorker()
{
    startThreadPriority = QThread::InheritPriority;
}

HashThreadedWorker::~HashThreadedWorker()
{

}

void HashThreadedWorker::setChunkSize(int size)
{
    chunkSize = size;
}

void HashThreadedWorker::setHash(const QString hash_needed)
{
    this->hash_needed = hash_needed;
}

void HashThreadedWorker::setColor(QColor color)
{
    lineColor = color;
}

void HashThreadedWorker::setNumber(int currentThreadNumber)
{
    number = currentThreadNumber;
}

void HashThreadedWorker::setCurrentI(int *i)
{
    current_i = i;
//    state = IDLE;
    state = IDLE;
}

int HashThreadedWorker::getNumber()
{
    return number;
}

void HashThreadedWorker::setMutex(QMutex* sharedMutex)
{
    mutex = sharedMutex;
}

void HashThreadedWorker::setIsOrdered(bool state)
{
    isOrdered = state;
}

void HashThreadedWorker::setStartPriority(QThread::Priority priority)
{
    startThreadPriority = priority;
}

QThread::Priority HashThreadedWorker::startPriority()
{
    return startThreadPriority;
}

void HashThreadedWorker::run()
{
    // WARNING: there is a little bug when you click Stop All at the beginning, layouts and threads will still exist
    // also, when you add more threads but not start all you can't do the work, cause you must start all threads
    // Some (hard) work that should be done in threads
    if (state == PAUSED)
    {
        // treat as resume
        qDebug() << "IDK was ist das, thread " << number << " somehow was paused, get out to work, th##ad!";
        state = RUNNING;
    }

    if (state == RUNNING)
    {
        qDebug() << "thread " << number << " is already running";
        return;
    }
    qDebug() << "start running thread" << number;
    state = RUNNING;
    int thisThreadIter;
    QString hash;

    while (state == RUNNING)
    {
        mutex->lock();
        thisThreadIter = *current_i;
        if (not isOrdered)
        {
            *current_i += chunkSize;
            mutex->unlock();
            doHardCalc(thisThreadIter, thisThreadIter + chunkSize, hash);
        }
        else {
            doHardCalc(thisThreadIter, thisThreadIter + chunkSize, hash);
            *current_i += chunkSize;
            mutex->unlock();
        }
        // qDebug() << QThread::currentThread()->priority();
        if (isCancelled())
        {
            break;
        }
    }
    qDebug() << "stoped running thread "<< number;
    emit finished();
}

void HashThreadedWorker::pause()
{
    auto const dispatcher = QThread::currentThread()->eventDispatcher();
    if (!dispatcher) {
        qCritical() << "thread " << number << " with no dispatcher";
        return;
    }

    if (state != RUNNING)
        return;

    state = PAUSED;
    qDebug() << "paused thread " << number;
    do {
        dispatcher->processEvents(QEventLoop::WaitForMoreEvents);
    } while (state == PAUSED);
}

void HashThreadedWorker::resume()
{
    if (state == PAUSED)
    {
        state = RUNNING;
        qDebug() << "resumed thread " << number;
    }
}

void HashThreadedWorker::cancel()
{
    if (state != IDLE)
    {
        state = IDLE;
        qDebug() << "cancelled thread " << number;
    }
}

bool HashThreadedWorker::isCancelled()
{
    auto const dispatcher = QThread::currentThread()->eventDispatcher();
    if (!dispatcher)
    {
        qCritical() << "thread " << number << " with no dispatcher";
        return false;
    }
    dispatcher->processEvents(QEventLoop::AllEvents);
    return state == IDLE;
}

void HashThreadedWorker::doHardCalc(int thisThreadIter, int endIter, QString &hash)
{
    for (int i = thisThreadIter; i < endIter; i++)
    {
        if ((hash = QString::fromStdString(sha256(std::to_string(i)))) == hash_needed)
        {
            state = IDLE;
            // *current_i = thisThreadIter;
            // finish threads in that way so everything ended fine and there was an answer
            emit gotAnswer(i, hash);
            thisThreadIter = i;
            break;
        }
    }
    emit calc_step(number, thisThreadIter, hash, lineColor);
}
