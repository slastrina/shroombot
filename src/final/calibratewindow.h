#ifndef CALIBRATEWINDOW_H
#define CALIBRATEWINDOW_H

#include <QWidget>

namespace Ui {
class CalibrateWindow;
}

class CalibrateWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrateWindow(QWidget *parent = 0);
    ~CalibrateWindow();

private:
    Ui::CalibrateWindow *ui;
};

#endif // CALIBRATEWINDOW_H
