#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    emit sigCloseWindow();
    exit(EXIT_SUCCESS);
}

void MainWindow::on_pushButton_clicked()
{
    emit sigBtnPushed();

}

void MainWindow::on_sbMaxRad_valueChanged(int arg1)
{
    emit maxRadSig(arg1);
}

void MainWindow::on_sbMinRad_valueChanged(int arg1)
{
    emit minRadSig(arg1);
}

void MainWindow::on_sbBright_valueChanged(int arg1)
{
    emit lightLevelSig(arg1);
}

//void MainWindow::on_pushButton_2_clicked()
//{
//    endoDetect();
//}

void MainWindow::on_actionReCalibrate_triggered()
{
    cw.show();
}

void MainWindow::on_actionManual_Controls_triggered()
{
    mcw.show();
}

void MainWindow::on_actionConnect_triggered()
{
    cnw.show();
}

void MainWindow::updateCamUI(QImage qimgOriginal, QImage qimgProcessed)
{
    ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgProcessed));
    ui->lblOriginal->setPixmap(QPixmap::fromImage(qimgOriginal));
}

void MainWindow::updatelblOriginal(QImage qimgOriginal)
{
      ui->lblProcessed->setPixmap(QPixmap::fromImage(qimgOriginal));
}
