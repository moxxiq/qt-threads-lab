#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QSlider>
#include <QApplication>
#include "hashthreadedworker.h"
#include "threadcontroller.h"
#include <string>

#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void appendLine(int threadNumber, int current_i, QString currentHash, QColor lineColor);
    void on_pushButtonAddThread_clicked();
    void alertAnswer(int number, QString hash);
    void work_done();

private:
    Ui::MainWindow *ui;
    ThreadController workController;
    // give every thread a number
    int currentThreadNumber;
    int currentSharedI;

    QString lastAnswer;
    QElapsedTimer timer;
};
#endif // MAINWINDOW_H
