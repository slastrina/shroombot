#include "manualcontrolswindow.h"
#include "ui_manualcontrolswindow.h"

ManualControlsWindow::ManualControlsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualControlsWindow)
{
    ui->setupUi(this);
}

ManualControlsWindow::~ManualControlsWindow()
{
    delete ui;
}
