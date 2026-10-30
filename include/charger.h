// charger.h - CC-CV charger (Drew)
#pragma once
#include <string>
 
class Pack;  // forward declaration; the full class is in pack.h
 
// Project-wide sign convention:
//   positive current = discharge, negative current = charge
 
class Charger {
public:
    enum Mode { CC, CV, DONE };
 
    Charger(double ccAmps = 3.0, double cvVolts = 4.2, double cutoffAmps = 0.15);
 
    // Returns the current to apply this timestep (negative = charging).
    // Call once per timestep while charging.
    double getChargeCurrent(const Pack& pack);
 
    Mode getMode() const;
    std::string getModeName() const;  // "CC", "CV", or "DONE" for the status line
    bool isDone() const;
    void reset();  // back to CC, ready for a new charge
 
private:
    double ccCurrent;      // A, current held during CC
    double cvVoltage;      // V, per-cell target during CV
    double cutoffCurrent;  // A, charge ends once current tapers below this
    Mode mode;
    double current;        // A, last commanded current (CV adjusts it each step)
};