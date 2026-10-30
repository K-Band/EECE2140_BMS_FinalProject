// energy_tracker.cpp - Wh in/out, efficiency, cycle count (Drew)
#include "energy_tracker.h"
 
EnergyTracker::EnergyTracker(double capAh)
    : whIn(0.0),
      whOut(0.0),
      chargeThroughputAh(0.0),
      capacityAh(capAh) {}
 
void EnergyTracker::update(double voltage, double current, double dt) {
    double hours = dt / 3600.0;
 
    if (current > 0.0) {           // discharging: energy leaves the pack
        whOut += voltage * current * hours;
        chargeThroughputAh += current * hours;
    } else if (current < 0.0) {    // charging: energy goes into the pack
        whIn += voltage * (-current) * hours;
    }
}
 
double EnergyTracker::getWhIn() const {
    return whIn;
}
 
double EnergyTracker::getWhOut() const {
    return whOut;
}
 
// Only meaningful after charging and discharging over the same SOC range.
double EnergyTracker::getEfficiency() const {
    if (whIn <= 0.0) {
        return 0.0;
    }
    return 100.0 * whOut / whIn;
}
 
// One cycle = one full capacity's worth of charge discharged.
double EnergyTracker::getCycleCount() const {
    if (capacityAh <= 0.0) {
        return 0.0;
    }
    return chargeThroughputAh / capacityAh;
}