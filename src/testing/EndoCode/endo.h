#ifndef ENDO_H
#define ENDO_H

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
//#include <QImage>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <algorithm>
#include <iostream>

//#include "gantry_helper.h"

#define camCenterOffset_X 126.0 // distance from X home to center for camera detection
#define xRatio 0.7988 // pixel to mm ratio for x = 270/388
#define yRatio 0.9112 // pixel to mm ratio for y = 400/439
#define xOffset 290 // x offset from gantry origin
#define yOffset 440 // y offset from gantry origin

class Endo
{
    //Q_OBJECT
public:
    Endo();
    void endoDetect();
private:
    cv::VideoCapture capEndo;
    int minRadEndo;
    int maxRadEndo;

signals:

public slots:
};

#endif // ENDO_H
