#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <opencv2/core/core.hpp>
#include <QtCore>

#include "mainwindow.h"
#include "logicthread.h"

int detectNumAttachedCvCameras()
{
    int number = 0;
    while (true)
    {
        qDebug( "the number is: %d", number );

        cv::VideoCapture cap;
        cap.open(number);
        if (!cap.isOpened())
        {
            qDebug( "failed" );

            break;
        }
        else
        {
            number++;
            cap.release();
        }
    }

    qDebug( "the number is: %d", number );

    return number;
}

void checkCamerasConnected()
{
    int cams = detectNumAttachedCvCameras();
    while (cams != 1)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Incorrect Number of Cameras Detected");
        msgBox.setInformativeText("Press Retry to redetect video streams");
        QPushButton *retryButton = msgBox.addButton("retry", QMessageBox::ActionRole);
        QPushButton *abortButton = msgBox.addButton(QMessageBox::Abort);

        msgBox.exec();

        if (msgBox.clickedButton() == retryButton)
        {

        } else if (msgBox.clickedButton() == abortButton)
        {
            exit(EXIT_FAILURE);
        }
        cams = detectNumAttachedCvCameras();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//  checkCamerasConnected(); not working as its not possible to enumerate cameras

//  StateMachine and State Declarations
    QStateMachine machine;
    QState *Wait = new QState();
    QState *Init = new QState();
    QState *CamDetect = new QState();
    QState *Process = new QState();
    QState *MoveToMushroom = new QState();
    QState *EndoLocate = new QState();
    QState *GetSeal = new QState();
    QState *MushroomDisplacement = new QState();
    QState *RaiseMushroom = new QState();
    QState *MoveToBin = new QState();

//  Creating and Starting the 'SerialLogicThread' Thread
    LogicThread s;
    s.start(QThread::LowestPriority);

//  StateMachine Transitions
    Wait->addTransition(&s, SIGNAL(sigInit()), Init);
    Init->addTransition(&s, SIGNAL(sigCamDetect()), CamDetect);
    Init->addTransition(&s, SIGNAL(sigWait()), Wait);
    CamDetect->addTransition(&s, SIGNAL(sigProcess()), Process);
    CamDetect->addTransition(&s, SIGNAL(sigWait()), Wait);
    Process->addTransition(&s, SIGNAL(sigMoveToMushroom()), MoveToMushroom);
    Process->addTransition(&s, SIGNAL(sigCamDetect()),Init);
    MoveToMushroom->addTransition(&s, SIGNAL(sigEndoLocate()), EndoLocate);
    MoveToMushroom->addTransition(&s, SIGNAL(sigWait()), Wait);
    EndoLocate->addTransition(&s, SIGNAL(sigGetSeal()), GetSeal);
    EndoLocate->addTransition(&s, SIGNAL(sigProcess()), Process);
    EndoLocate->addTransition(&s, SIGNAL(sigWait()), Wait);
    GetSeal->addTransition(&s, SIGNAL(sigMushroomDisplacement()), MushroomDisplacement);
    GetSeal->addTransition(&s, SIGNAL(sigProcess()), Process);
    GetSeal->addTransition(&s, SIGNAL(sigWait()), Wait);
    MushroomDisplacement->addTransition(&s, SIGNAL(sigRaiseMushroom()), RaiseMushroom);
    MushroomDisplacement->addTransition(&s, SIGNAL(sigWait()), Wait);
    RaiseMushroom->addTransition(&s, SIGNAL(sigMoveToBin()), MoveToBin);
    RaiseMushroom->addTransition(&s, SIGNAL(sigWait()), Wait);
    MoveToBin->addTransition(&s, SIGNAL(sigProcess()), Process);
    MoveToBin->addTransition(&s, SIGNAL(sigWait()), Wait);

//  Add States to StateMachine
    machine.addState(Wait);
    machine.addState(Init);
    machine.addState(CamDetect);
    machine.addState(Process);
    machine.addState(MoveToMushroom);
    machine.addState(EndoLocate);
    machine.addState(GetSeal);
    machine.addState(MushroomDisplacement);
    machine.addState(RaiseMushroom);
    machine.addState(MoveToBin);

//  Set the Initial State
    machine.setInitialState(Wait);

//  Start the StateMachine
    machine.start();

//  Create the 'MainWindow' UI
    MainWindow w;

//  Thread Connections
    QObject::connect(&w, SIGNAL(sigBtnPushed()),&s, SLOT(slotBtnPushed()));
    QObject::connect(&w, SIGNAL(sigCloseWindow()), &s, SLOT(slotCloseWindow()));
    QObject::connect(&s, SIGNAL(updateCamUI(QImage, QImage)), &w, SLOT(updateCamUI(QImage, QImage)));
    QObject::connect(&s, SIGNAL(updatelblOriginal(QImage)), &w, SLOT(updatelblOriginal(QImage)));

//  State Connections
    QObject::connect(Wait, SIGNAL(entered()), &s, SLOT(slotWaitEntered()));
    QObject::connect(Init, SIGNAL(entered()), &s, SLOT(slotInitEntered()));
    QObject::connect(CamDetect, SIGNAL(entered()), &s, SLOT(slotCamDetectEntered()));
    QObject::connect(Process, SIGNAL(entered()), &s, SLOT(slotProcessEntered()));
    QObject::connect(MoveToMushroom, SIGNAL(entered()), &s, SLOT(slotMoveToMushroomEntered()));
    QObject::connect(EndoLocate, SIGNAL(entered()), &s, SLOT(slotEndoLocateEntered()));
    QObject::connect(GetSeal, SIGNAL(entered()), &s, SLOT(slotGetSealEntered()));
    QObject::connect(MushroomDisplacement, SIGNAL(entered()), &s, SLOT(slotMushroomDisplacementEntered()));
    QObject::connect(RaiseMushroom, SIGNAL(entered()), &s, SLOT(slotRaiseMushroomEntered()));
    QObject::connect(MoveToBin, SIGNAL(entered()), &s, SLOT(slotMoveToBinEntered()));

//  Show the MainWindow
    w.show();

    return a.exec();
}
