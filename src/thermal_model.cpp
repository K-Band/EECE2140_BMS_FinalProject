#include "thermal_model.h"

ThermalModel::ThermalModel(double coolingCoeff, double thermalCapacity, double ambientC)
    : k(coolingCoeff), cth(thermalCapacity), tAmb(ambientC) {}

double ThermalModel::step(double temperature, double /*current*/, double /*resistance*/,
                          double /*dt*/) const {
    // TODO: return T + (I^2 * R - k * (T - tAmb)) * dt / cth
    return temperature;
}

void ThermalModel::setAmbient(double tempC) { tAmb = tempC; }
double ThermalModel::getAmbient() const { return tAmb; }