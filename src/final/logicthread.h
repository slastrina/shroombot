#ifndef LOGICTHREAD_H
#define LOGICTHREAD_H

#include <QObject>
#include <QtCore>
#include <QStateMachine>
#include <QState>
#include <QEventTransition>
#include <QFinalState>
#include <QHistoryState>
#include <QSignalTransition>
#include <QtSerialPort/QSerialPort>
#include <QIODevice>
#include <QFlags>
#include <QImage>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <algorithm>
#include <iostream>

#include "qextserialport.h"

#include "gantry_helper.h"

#define camCenterOffset_X 126.0 // distance from X home to center for camera detection
#define xRatio 0.7988 // pixel to mm ratio for x = 270/388
#define yRatio 0.9112 // pixel to mm ratio for y = 400/439
#define xOffset 290 // x offset from gantry origin
#define yOffset 440 // y offset from gantry origin


class LogicThread : public QThread
{
    Q_OBJECT
public:
    //explicit SerialLogicThread(QObject *parent=0);
    LogicThread();
    ~LogicThread();
    bool serialIsOK(int time);
    void run();
    int exec();

private:
    bool moveEntered;
    float currX, desX;
    float currY, desY;
    float currZ, desZ;

//  STATEMACHINE
    enum StateName {WAIT, INIT, CAMDETECT, PROCESS, MOVETOMUSHROOM, ENDOLOCATE, GETSEAL, MUSHROOMDISPLACEMENT, RAISEMUSHROOM, MOVETOBIN};
    StateName state;

//  SERIAL
    //QSerialPort serial; //OLD WAY
    QextSerialPort serial;
    QString portName, sendLine;
    void serialDump();//happuy?
    bool lineReady;
    bool timedOut;

//  GANTRY
    bool linear_move(char c, double x, int f);
    bool linear_move(char c, double x, char cc, double y, int f);
    bool linear_move(char c, double x, char cc, double y, char ccc, double z, int f);

//  VISION
    cv::VideoCapture capWebcam;
    cv::VideoCapture capEndo;
    void birdDetect();
    void endoDetect();
    QImage convertOpenCVMatToQtQImage(cv::Mat mat);
    int minRad,maxRad,lightLevel,maxHit;
    std::vector<cv::Vec3f> hitList, hitListOrdered;
    std::vector<int> hitNumbers, hitNumbersOrdered;
    std::vector<cv::Point2f> adjHitList;
    cv::Vec3f toMove;
    float toMoveX, toMoveY;
    bool convertToReality();
    bool nextTopHit();
    bool endoOperation;
  //  cv::Mat imgOriginal, imgHSV, imgThreshLow, imgThreshHigh, imgThresh, imgProcessed, imgDark;


signals:

//  State Transition Signals
    void sigWait();
    void sigInit();
    void sigCamDetect();
    void sigProcess();
    void sigMoveToMushroom();
    void sigEndoLocate();
    void sigGetSeal();
    void sigMushroomDisplacement();
    void sigRaiseMushroom();
    void sigMoveToBin();

//  UI Signals
    void updateCamUI(QImage qimgOriginal, QImage qimgProcessed);
    void updatelblOriginal(QImage qimgOriginal);


public slots:

    void slotTimedOut();
    void slotLineReady();

//  State Entered Slots
    void slotWaitEntered();
    void slotInitEntered();
    void slotCamDetectEntered();
    void slotProcessEntered();
    void slotEndoLocateEntered();
    void slotMoveToMushroomEntered();
    void slotGetSealEntered();
    void slotMushroomDisplacementEntered();
    void slotRaiseMushroomEntered();
    void slotMoveToBinEntered();

//  UI slots
    void slotBtnPushed();
    void slotCloseWindow();

};


#endif // LOGICTHREAD_H
