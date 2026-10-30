#ifndef CELL_H
#define CELL_H

#include "ocv_table.h"
#include "thermal_model.h"

// Owner: Kavi
// One lithium-ion cell.
// Current sign convention (used everywhere in the project):
//   current > 0  -> discharging (current leaves the cell)
//   current < 0  -> charging    (current enters the cell)
class Cell {
public:
    Cell(double capacityAh = 3.0, double baseResistance = 0.02, double startSOC = 1.0);

    void update(double current, double dt);   // advance SOC and temperature by dt seconds
    double getTerminalVoltage() const;        // OCV - I * R
    double getOCV() const;
    double getResistance() const;             // may depend on temperature
    double getSOC() const;
    double getTemperature() const;
    double getCapacity() const;

private:
    double capacityAh;
    double soc;
    double baseResistance;   // internal resistance at 25 deg C (ohms)
    double temperature;      // deg C
    double presentCurrent;   // A, most recent current applied
    OcvTable ocvTable;
    ThermalModel thermal;
};

#endif