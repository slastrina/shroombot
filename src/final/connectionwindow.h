#ifndef CONNECTIONWINDOW_H
#define CONNECTIONWINDOW_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class ConnectionWindow;
}

class ConnectionWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionWindow(QWidget *parent = 0);
    ~ConnectionWindow();

private slots:

    void on_pbConnect_clicked();

    void on_lnName_textEdited(const QString &arg1);

    void on_pbSend_clicked();

    void on_lnSend_editingFinished();

private:
    Ui::ConnectionWindow *ui;
    QSerialPort serial;
    QString portName, sendLine;
};

#endif // CONNECTIONWINDOW_H
