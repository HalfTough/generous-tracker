#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <QThread>


class MainProgram : public QObject{
    Q_OBJECT

    int argc, ext=0;
    char **argv;
    void exitApp(int ret);
public:
    MainProgram(int argc, char **argv, QObject *parent = Q_NULLPTR) :QObject(parent), argc(argc),argv(argv){
    }
    int getExitCode(){ return ext; }
public slots:
    void run();
};

#endif
