#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    currentThreadNumber = 1;
    connect(&workController, &ThreadController::finished, this, &MainWindow::work_done);
    currentSharedI = 0;
    lastAnswer = "undefined";
    timer.invalidate();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::appendLine(int threadNumber, int current_i, QString currentHash, QColor lineColor)
{
    ui->textBrowser->setTextColor(lineColor);
    ui->textBrowser->append("Thread " + QString::number(threadNumber) + ": " +
                            QString::number(current_i) + " = " + currentHash);
}

void MainWindow::on_pushButtonAddThread_clicked()
{
    QThread* thread = new QThread();
    HashThreadedWorker* newbie = new HashThreadedWorker();
    newbie->setNumber(currentThreadNumber++);

    workController.add_task(newbie, thread);

    // creating line of buttons layout and others
    QBoxLayout* newbie_layout = new QHBoxLayout();

    QLabel *threadLabel = new QLabel("Thread " + QString::number(newbie->getNumber()));
    QPushButton *startThreadButton = new QPushButton("Start");
    QPushButton *pauseThreadButton = new QPushButton("Pause");
    QPushButton *resumeThreadButton = new QPushButton("Resume");
    QLabel *priorityValueLabel = new QLabel(workController.QStrFromPriority(QThread::InheritPriority));
    QSlider *priorityThreadSlider = new QSlider(Qt::Horizontal);
    priorityThreadSlider->setMaximumWidth(128);
    priorityThreadSlider->setValue(7);
    priorityThreadSlider->setMinimum(0);
    priorityThreadSlider->setMaximum(6);
    priorityThreadSlider->setTickPosition(QSlider::TicksBothSides);
    priorityThreadSlider->setTickInterval(1);
    QPushButton *stopThreadButton = new QPushButton("Stop");

    // make resume invisible by default
    resumeThreadButton->setVisible(false);
    // only set state on change
    connect(ui->checkBoxOrder, &QCheckBox::stateChanged,
            newbie, [=](int state)
    {
        newbie->setIsOrdered(bool(state));
    });
    connect(startThreadButton, &QPushButton::clicked,
            thread, [=] ()
    {
        ui->spinBox->setReadOnly(true);
        ui->spinBox_chunk->setReadOnly(true);
        int guess = ui->spinBox->value();
        QString correct_hash = QString::fromStdString(sha256(std::to_string(guess)));
        newbie->setHash(correct_hash);
        newbie->setCurrentI(&currentSharedI);
        // set chunk size
        newbie->setChunkSize(ui->spinBox_chunk->value());
        // don't forget to change state
        newbie->setIsOrdered(ui->checkBoxOrder->isChecked());
        if (not timer.isValid())
        {
            qDebug() << "Timer started";
            timer.start();
        }
        thread->start(newbie->startPriority());
        qDebug() << "Thread " << newbie->getNumber() << "started with priority " << thread->priority();
    });
    connect(pauseThreadButton, &QPushButton::clicked,
            newbie, &HashThreadedWorker::pause);
    connect(resumeThreadButton, &QPushButton::clicked,
            newbie, &HashThreadedWorker::resume);
    connect(priorityThreadSlider, &QSlider::valueChanged,
            this, [=](int value){
        priorityValueLabel->setText(workController.QStrFromPriority(QThread::Priority(value)));
        if (thread->isRunning())
        {
            thread->setPriority(QThread::Priority(value));
        } else
        {
            newbie->setStartPriority(QThread::Priority(value));
        }
    });
    connect(stopThreadButton, &QPushButton::clicked,
            newbie, &HashThreadedWorker::cancel);
    connect(ui->pushButtonStartAll, &QPushButton::clicked,
            startThreadButton, &QPushButton::clicked);
    connect(ui->pushButtonStopAll, &QPushButton::clicked,
            newbie, &HashThreadedWorker::cancel);

    // ui
    // hide start button on click
    connect(startThreadButton, &QPushButton::clicked,
            this, [=]()
    {
        startThreadButton->setVisible(false);
    });
    // hide pause button on click and show resume button
    connect(pauseThreadButton, &QPushButton::clicked,
            this, [=]()
    {
        resumeThreadButton->setVisible(true);
        pauseThreadButton->setVisible(false);
    });
    // the opposite to the previous one
    connect(resumeThreadButton, &QPushButton::clicked,
            this, [=]()
    {
        pauseThreadButton->setVisible(true);
        resumeThreadButton->setVisible(false);
    });


    newbie_layout->addWidget(threadLabel);
    newbie_layout->addWidget(startThreadButton);
    newbie_layout->addWidget(pauseThreadButton);
    newbie_layout->addWidget(resumeThreadButton);
    newbie_layout->addWidget(priorityValueLabel);
    newbie_layout->addWidget(priorityThreadSlider);
    newbie_layout->addWidget(stopThreadButton);

    ui->verticalLayout->addLayout(newbie_layout);

    // on every algorithm step write a new line with details
    connect(newbie, &HashThreadedWorker::calc_step,
            this, &MainWindow::appendLine);
    // when work is finished -- remove all the buttons, labels and layout from UI and delete them
    // using fancy lambda functions as a slot
    // this deletes the only row (call it layout) that finished, not all of them
    connect(newbie, &HashThreadedWorker::finished,
            this,
            [=]()
    {
        // remove all the widgets and a layout after finished work
        if (newbie_layout != nullptr)
        {
            QLayoutItem* item;
            while ( (item = newbie_layout->takeAt(0)) != nullptr )
            {
                // this can be used only for a widgets (buttons, labels etc.)
                // for a layouts you should use apropriate functions and clear them if it's needed
                item->widget()->deleteLater();
            }
            // why deleteLater? -> https://forum.qt.io/topic/91180/delete-vs-deletelater/2
            newbie_layout->deleteLater();
        }
    });
    connect(newbie, &HashThreadedWorker::gotAnswer,
            this, &MainWindow::alertAnswer);
    connect(newbie, &HashThreadedWorker::gotAnswer,
            ui->pushButtonStopAll, &QPushButton::clicked);

}

void MainWindow::alertAnswer(int number, QString hash)
{
    qint64 timeMsec = timer.elapsed();
    timer.invalidate();
    currentSharedI = 0;
    ui->textBrowser->setTextColor(QColorConstants::Black);
    lastAnswer = QString::number(number) + " = " + hash + " time: " +
            QString::number(timeMsec/1000.0) + " sec";
    ui->textBrowser->append("Finished!!! " + lastAnswer);
}

void MainWindow::work_done()
{
    currentSharedI = 0;
    ui->spinBox->setReadOnly(false);
    ui->spinBox_chunk->setReadOnly(false);
    qDebug() << "Work done";
    QMessageBox msgBox;
    msgBox.setWindowTitle("Answer");
    msgBox.setText("Answer is " + lastAnswer);
    lastAnswer = "undefined";
    msgBox.exec();
}
