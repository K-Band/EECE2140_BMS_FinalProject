// charger.cpp - CC-CV charger (Drew)
#include "charger.h"
#include "pack.h"
 
Charger::Charger(double ccAmps, double cvVolts, double cutoffAmps)
    : ccCurrent(ccAmps),
      cvVoltage(cvVolts),
      cutoffCurrent(cutoffAmps),
      mode(CC),
      current(0.0) {}
 
double Charger::getChargeCurrent(const Pack& pack) {
    const double cvGain = 5.0;  // A per V; lower this if CV oscillates
 
    if (mode == DONE) {
        return 0.0;
    }
 
    // The highest cell sets the limit, since all series cells share one current.
    double vMax = pack.getMaxCellVoltage();
 
    if (mode == CC) {
        if (vMax < cvVoltage) {
            current = ccCurrent;
            return -current;
        }
        mode = CV;  // highest cell reached the CV voltage; switch this step
    }
 
    // CV: nudge the current so the highest cell holds at cvVoltage.
    // Above target -> reduce current. Below target -> raise it (capped at ccCurrent).
    current -= cvGain * (vMax - cvVoltage);
    if (current > ccCurrent) current = ccCurrent;
    if (current < 0.0)       current = 0.0;
 
    if (current <= cutoffCurrent) {  // current has tapered out: charge complete
        mode = DONE;
        current = 0.0;
        return 0.0;
    }
    return -current;
}
 
Charger::Mode Charger::getMode() const {
    return mode;
}
 
std::string Charger::getModeName() const {
    switch (mode) {
        case CC:   return "CC";
        case CV:   return "CV";
        case DONE: return "DONE";
    }
    return "UNKNOWN";
}
 
bool Charger::isDone() const {
    return mode == DONE;
}
 
void Charger::reset() {
    mode = CC;
    current = 0.0;
}