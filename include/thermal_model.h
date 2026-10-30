#ifndef THERMAL_MODEL_H
#define THERMAL_MODEL_H

// Owner: Kavi
// Lumped thermal model for one cell:
//   T_next = T + (I^2 * R - k * (T - T_amb)) * dt / Cth
class ThermalModel {
public:
    ThermalModel(double coolingCoeff = 0.1, double thermalCapacity = 50.0, double ambientC = 25.0);

    // Returns the cell temperature (deg C) after one time step.
    double step(double temperature, double current, double resistance, double dt) const;
    void setAmbient(double tempC);
    double getAmbient() const;

private:
    double k;      // cooling coefficient (W/K)
    double cth;    // thermal capacity (J/K)
    double tAmb;   // ambient temperature (deg C)
};

#endif