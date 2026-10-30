// protection.h - warning/fault checks with a latching fault (Drew)
#pragma once
#include <string>
#include <vector>
 
class Pack;  // forward declaration; the full class is in pack.h
 
// Voltages are per cell (V), current in A, temperature in C,
// SOC as a fraction 0.0-1.0. Positive current = discharge.
 
class Protection {
public:
    enum Check { OVER_VOLTAGE, UNDER_VOLTAGE, OVER_CURRENT, OVER_TEMP, LOW_SOC, NUM_CHECKS };
    enum Level { OK, WARNING, FAULT };
 
    Protection();
 
    // Runs all five checks. A fault latches until resetFault() is called.
    // The caller (Simulator) opens the contactor when this returns FAULT.
    Level check(const Pack& pack, double current);
 
    bool setLimit(int which, Level level, double value);  // false if invalid
    double getLimit(int which, Level level) const;        // lets T7 show current limits
    bool isFaulted() const;
    void resetFault();  // re-trips on the next check if the condition remains
    std::string getFaultReason() const;  // needed by the event log
    const std::vector<std::string>& getActiveWarnings() const;
 
    static std::string getCheckName(int which);  // "Overvoltage", etc., for T7 and messages
 
private:
    double warningLimits[NUM_CHECKS];
    double faultLimits[NUM_CHECKS];
    bool faulted;
    std::string faultReason;
    std::vector<std::string> activeWarnings;
};