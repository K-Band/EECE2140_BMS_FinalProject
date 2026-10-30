// protection.cpp - warning/fault checks with a latching fault (Drew)
#include "protection.h"
#include "pack.h"
#include <cmath>
#include <iomanip>
#include <sstream>
 
Protection::Protection() : faulted(false) {
    // Default limits for a typical Li-ion cell; adjust to match the cell parameters.
    warningLimits[OVER_VOLTAGE]  = 4.25;  faultLimits[OVER_VOLTAGE]  = 4.30;  // V per cell
    warningLimits[UNDER_VOLTAGE] = 3.00;  faultLimits[UNDER_VOLTAGE] = 2.70;  // V per cell
    warningLimits[OVER_CURRENT]  = 5.0;   faultLimits[OVER_CURRENT]  = 7.5;   // A
    warningLimits[OVER_TEMP]     = 45.0;  faultLimits[OVER_TEMP]     = 60.0;  // C
    warningLimits[LOW_SOC]       = 0.10;  faultLimits[LOW_SOC]       = 0.05;  // fraction
}
 
Protection::Level Protection::check(const Pack& pack, double current) {
    static const char* units[NUM_CHECKS] = {"V", "V", "A", "C", "%"};
 
    double values[NUM_CHECKS];
    values[OVER_VOLTAGE]  = pack.getMaxCellVoltage();
    values[UNDER_VOLTAGE] = pack.getMinCellVoltage();
    values[OVER_CURRENT]  = std::fabs(current);
    values[OVER_TEMP]     = pack.getMaxTemperature();
    values[LOW_SOC]       = pack.getSOC();
 
    bool discharging = current > 0.0;
    activeWarnings.clear();
 
    for (int i = 0; i < NUM_CHECKS; i++) {
        // Undervoltage and low SOC trip when the value drops BELOW the limit.
        bool lowSide = (i == UNDER_VOLTAGE || i == LOW_SOC);
 
        // Low-side checks only apply while discharging.
        // Otherwise a nearly empty pack would fault and could never be recharged.
        if (lowSide && !discharging) {
            continue;
        }
 
        bool overFault = lowSide ? values[i] < faultLimits[i]   : values[i] > faultLimits[i];
        bool overWarn  = lowSide ? values[i] < warningLimits[i] : values[i] > warningLimits[i];
        if (!overFault && !overWarn) {
            continue;
        }
 
        // Build a message like "Overvoltage FAULT: 4.31 V (limit 4.30 V)"
        double limit = overFault ? faultLimits[i] : warningLimits[i];
        double scale = (i == LOW_SOC) ? 100.0 : 1.0;  // show SOC as a percent
        std::ostringstream msg;
        msg << std::fixed << std::setprecision(2)
            << getCheckName(i) << (overFault ? " FAULT: " : " WARNING: ")
            << values[i] * scale << " " << units[i]
            << " (limit " << limit * scale << " " << units[i] << ")";
 
        if (overFault) {
            if (!faulted) {  // latch the first fault and record why
                faulted = true;
                faultReason = msg.str();
            }
        } else {
            activeWarnings.push_back(msg.str());
        }
    }
 
    if (faulted) return FAULT;
    if (!activeWarnings.empty()) return WARNING;
    return OK;
}
 
bool Protection::setLimit(int which, Level level, double value) {
    if (which < 0 || which >= NUM_CHECKS || value < 0.0) {
        return false;
    }
    if (level == WARNING) {
        warningLimits[which] = value;
    } else if (level == FAULT) {
        faultLimits[which] = value;
    } else {
        return false;
    }
    return true;
}
 
double Protection::getLimit(int which, Level level) const {
    if (which < 0 || which >= NUM_CHECKS) {
        return 0.0;
    }
    if (level == WARNING) return warningLimits[which];
    if (level == FAULT)   return faultLimits[which];
    return 0.0;
}
 
bool Protection::isFaulted() const {
    return faulted;
}
 
void Protection::resetFault() {
    faulted = false;
    faultReason.clear();
}
 
std::string Protection::getFaultReason() const {
    return faultReason;
}
 
const std::vector<std::string>& Protection::getActiveWarnings() const {
    return activeWarnings;
}
 
std::string Protection::getCheckName(int which) {
    switch (which) {
        case OVER_VOLTAGE:  return "Overvoltage";
        case UNDER_VOLTAGE: return "Undervoltage";
        case OVER_CURRENT:  return "Overcurrent";
        case OVER_TEMP:     return "Overtemperature";
        case LOW_SOC:       return "Low SOC";
    }
    return "Unknown";
}