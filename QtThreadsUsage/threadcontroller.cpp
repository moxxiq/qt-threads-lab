#include "threadcontroller.h"

ThreadController::ThreadController(QObject *parent) : QObject(parent)
{
    threads_counter = 0;
    sharedCurrentNumber = 0;
}

ThreadController::~ThreadController()
{

}

void ThreadController::add_task(HashThreadedWorker *task, QThread* thread)
{
    task->setParent(nullptr);
    task->setMutex(&mutex);
    thread->setObjectName("Worker thread " + QString::number(task->getNumber()));

    task->setColor(colorSet[(task->getNumber() - 1) % colorQuantity]);

    task->moveToThread(thread);
    connect(thread, &QThread::started, task, &HashThreadedWorker::run);
    connect(task, &HashThreadedWorker::finished, task, &HashThreadedWorker::deleteLater);
    connect(task, &HashThreadedWorker::destroyed, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(thread, &QThread::destroyed, this, [=]()
    {
        int res = threads.removeOne(thread);
        qDebug() << "Removed QThread: " << bool(res);
    });
    connect(thread, &QThread::destroyed, this, &ThreadController::isAllStopped);

    threads.push_back(thread);
}

QString ThreadController::QStrFromPriority(QThread::Priority priority)
{
    switch (priority) {
        case QThread::IdlePriority:
            return "Idle Priority";
        case QThread::LowestPriority:
            return "Lowest Priority";
        case QThread::LowPriority:
            return "Low Priority";
        case QThread::NormalPriority:
            return "Normal Priority";
        case QThread::HighPriority:
            return "High Priority";
        case QThread::HighestPriority:
            return "Highest Priority";
        case QThread::TimeCriticalPriority:
            return "Time CriticalPriority";
        case QThread::InheritPriority:
            return "Inherit Priority";
    };
    return "";
}

void ThreadController::isAllStopped()
{
    if (threads.count() == 0)
    {
        emit finished();
    }
}
