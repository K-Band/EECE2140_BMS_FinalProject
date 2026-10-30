#include "cell.h"

Cell::Cell(double capacityAh, double baseResistance, double startSOC)
    : capacityAh(capacityAh), soc(startSOC), baseResistance(baseResistance),
      temperature(25.0), presentCurrent(0.0) {}

void Cell::update(double current, double /*dt*/) {
    presentCurrent = current;
    // TODO: soc -= current * dt / (3600 * capacityAh), clamp to [0, 1],
    //       then temperature = thermal.step(temperature, current, getResistance(), dt)
}

double Cell::getTerminalVoltage() const {
    // TODO: return getOCV() - presentCurrent * getResistance()
    return getOCV();
}

double Cell::getOCV() const { return ocvTable.lookup(soc); }

double Cell::getResistance() const {
    // TODO: make resistance depend on temperature
    return baseResistance;
}

double Cell::getSOC() const { return soc; }
double Cell::getTemperature() const { return temperature; }
double Cell::getCapacity() const { return capacityAh; }