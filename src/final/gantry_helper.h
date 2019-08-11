#ifndef GANTRY_HELPER_H
#define GANTRY_HELPER_H

#include <QString>

QByteArray gantry_home();

QByteArray gantry_home(char c);

QByteArray gantry_home(char c, char cc);

QByteArray gantry_get_pos();

QByteArray gantry_servo_deg(int deg);

QByteArray gantry_servo_max();

QByteArray gantry_servo_min();

QByteArray gantry_pump_on();

QByteArray gantry_pump_off();

QByteArray gantry_valve1_on();

QByteArray gantry_valve1_off();

QByteArray gantry_valve2_on();

QByteArray gantry_valve2_off();

QByteArray gantry_linear_move(char c, double val, int f);

QByteArray gantry_linear_move(double x, double y, int f);

QByteArray gantry_linear_move(double x, double y, double z, int f);

QByteArray gantry_valve3_on();

QByteArray gantry_valve3_off();

#endif // GANTRY_HELPER_H
