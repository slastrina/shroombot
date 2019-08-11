#ifndef MANUALCONTROLSWINDOW_H
#define MANUALCONTROLSWINDOW_H

#include <QWidget>

namespace Ui {
class ManualControlsWindow;
}

class ManualControlsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ManualControlsWindow(QWidget *parent = 0);
    ~ManualControlsWindow();

private:
    Ui::ManualControlsWindow *ui;
};

#endif // MANUALCONTROLSWINDOW_H
