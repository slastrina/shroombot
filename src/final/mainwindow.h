#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QDebug>
#include <QTimer>
#include <QStateMachine>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <math.h>
#include <algorithm>  //std::max_element

#include "calibratewindow.h"
#include "manualcontrolswindow.h"
#include "connectionwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void sigBtnPushed();
    void sigCloseWindow();
    void maxRadSig(int arg1);
    void minRadSig(int arg1);
    void lightLevelSig(int arg1);

public slots:
    void updateCamUI(QImage qimgOriginal, QImage qimgProcessed);
    void updatelblOriginal(QImage qimgOriginal);

private slots:
    void on_actionQuit_triggered();

    void on_pushButton_clicked();

    void on_sbMaxRad_valueChanged(int arg1);

    void on_sbMinRad_valueChanged(int arg1);

    void on_sbBright_valueChanged(int arg1);

   // void on_pushButton_2_clicked();



    void on_actionReCalibrate_triggered();

    void on_actionManual_Controls_triggered();

    void on_actionConnect_triggered();


private:
    Ui::MainWindow *ui;
    CalibrateWindow cw;
    ManualControlsWindow mcw;
    ConnectionWindow cnw;

    QImage convertOpenCVMatToQtQImage(cv::Mat mat);


};

#endif // MAINWINDOW_H
