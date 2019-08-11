#include "connectionwindow.h"
#include "ui_connectionwindow.h"

ConnectionWindow::ConnectionWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionWindow)
{
    ui->setupUi(this);
}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}



void ConnectionWindow::on_pbConnect_clicked()
{
    serial.setPortName(portName);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);


    serial.open(QIODevice::ReadWrite);

    if(!serial.isOpen()) {
        ui->txtStatus->appendPlainText("Device Not Found");
    } else {
        ui->txtStatus->appendPlainText(portName + " Found!");
    }
}

void ConnectionWindow::on_lnName_textEdited(const QString &arg1)
{
    portName = arg1;
}

void ConnectionWindow::on_pbSend_clicked()
{
    if(!serial.isOpen()) {
        ui->txtStatus->appendPlainText("Device Not Connected");
    } else {
        serial.write(sendLine.toLocal8Bit());
        //wait somehow
        ui->txtStatus->appendPlainText("Message Sent!\nRecieved: \n"+serial.readAll());
    }
}

void ConnectionWindow::on_lnSend_editingFinished()
{
    sendLine = ui->lnSend->text();
}
