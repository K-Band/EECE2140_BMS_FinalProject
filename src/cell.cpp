#include "cell.h"

Cell::Cell(double capacityAh, double baseResistance, double startSOC)
    : capacityAh(capacityAh), soc(startSOC), baseResistance(baseResistance),
      temperature(25.0), presentCurrent(0.0) {}

// advances SOC and temperature
void Cell::update(double current, double dt) {
    presentCurrent = current;
    
    soc -= current * dt / (3600 * capacityAh);
    if(soc < 0) soc = 0;
    if(soc > 1.0) soc = 1.0;

    temperature = thermal.step(temperature, current, getResistance(), dt);
}

// returns the actual voltage of the battery the OCV - resistance losses
double Cell::getTerminalVoltage() const {
    return getOCV() - presentCurrent * getResistance();
}

double Cell::getOCV() const { return ocvTable.lookup(soc); }

// simple approximation for cell resistance changing with temperature
double Cell::getResistance() const {
    const double alpha = 0.01; // change in resistance per deg C
    
    double r = baseResistance * (1.0 + alpha * (25.0 - temperature));
    if(r < 0.5 * baseResistance) r = 0.5 * baseResistance; // floor for change in resistance
    
    return r;
}

double Cell::getSOC() const { return soc; }
double Cell::getTemperature() const { return temperature; }
double Cell::getCapacity() const { return capacityAh; }