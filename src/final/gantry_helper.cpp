#include "gantry_helper.h"

QByteArray gantry_home()
{
    QString strRet = "G28\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_home(char c)
{
    QString strRet;

    if(tolower(c) == 'x') strRet = "G28 X\n";
    else if(tolower(c) == 'y') strRet = "G28 Y\n";
    else if(tolower(c) == 'z') strRet = "G28 Z\n";
    else return NULL;

    return strRet.toLocal8Bit();
}

QByteArray gantry_home(char c, char cc)
{
    char temp1 = tolower(c);
    char temp2 = tolower(cc);

    QString strRet;

    if     (temp1 == 'x' && temp2 == 'y') strRet = "G28 X Y\n";
    else if(temp1 == 'x' && temp2 == 'z') strRet = "G28 X Z\n";
    else if(temp1 == 'y' && temp2 == 'x') strRet = "G28 X Y\n";
    else if(temp1 == 'y' && temp2 == 'z') strRet = "G28 Y Z\n";
    else if(temp1 == 'z' && temp2 == 'x') strRet = "G28 X Z\n";
    else if(temp1 == 'z' && temp2 == 'y') strRet = "G28 Y Z\n";
    else return NULL;

    return strRet.toLocal8Bit();
}

QByteArray gantry_get_pos()
{
    QString strRet = "M114\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_servo_deg(int deg)
{
    QString strRet = "M280 P0 " + deg + '\n';
    return strRet.toLocal8Bit();
}

QByteArray gantry_servo_max()
{
    QString strRet = "M280 P0 S0\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_servo_min()
{
    QString strRet = "M280 P0 S150\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_pump_on()
{
    QString strRet = "M42 P5 S0\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_pump_off()
{
    QString strRet = "M42 P5 S255\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_valve1_on()
{
    QString strRet = "M42 P9 S255\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_valve1_off()
{
    QString strRet = "M42 P9 S0\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_valve2_on()
{
    QString strRet = "M42 P6 S0\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_valve2_off()
{
    QString strRet = "M42 P6 S255\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_linear_move(char c, double val, int f) // move 1 axis
{
    QString strRet = "G01 " + QString(c) + QString::number(val) + 'F' + QString::number(f) + '\n';
    return strRet.toLocal8Bit();
}

QByteArray gantry_linear_move(double x, double y, int f) // move x and y axis
{
    QString strRet = "G01 X" + QString::number(x) + " Y" + QString::number(y) + " F" + QString::number(f) + '\n';
    return strRet.toLocal8Bit();
}

QByteArray gantry_linear_move(double x, double y, double z, int f) // move x and y and z axis
{
    QString strRet = "G01 X" + QString::number(x) + " Y" + QString::number(y) + " Z" + QString::number(z) + " F" + QString::number(f) + '\n';
    return strRet.toLocal8Bit();
}

QByteArray gantry_valve3_on()
{
    QString strRet = "M42 P49 S0\n";
    return strRet.toLocal8Bit();
}

QByteArray gantry_valve3_off()
{
    QString strRet = "M42 P49 S255\n";
    return strRet.toLocal8Bit();
}
