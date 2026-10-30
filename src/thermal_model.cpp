#include "thermal_model.h"

ThermalModel::ThermalModel(double coolingCoeff, double thermalCapacity, double ambientC)
    : k(coolingCoeff), cth(thermalCapacity), tAmb(ambientC) {}

// calculates the temperature of the pack from resistive heating and ambiient cooling
double ThermalModel::step(double temperature, double current, double resistance, double dt) const {
    double heating = current * current * resistance;   // W, I^2 R
    double cooling = k * (temperature - tAmb);         // W, heat lost to ambient
    return temperature + (heating - cooling) * dt / cth;
}

void ThermalModel::setAmbient(double tempC) { tAmb = tempC; }
double ThermalModel::getAmbient() const { return tAmb; }