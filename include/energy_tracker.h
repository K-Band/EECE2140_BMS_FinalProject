// energy_tracker.h - Wh in/out, efficiency, cycle count (Drew)
#pragma once
 
// Positive current = discharge, negative = charge.
 
class EnergyTracker {
public:
    EnergyTracker(double capacityAh = 3.0);
 
    // Pass the pack voltage and pack current every timestep.
    void update(double voltage, double current, double dt);
 
    double getWhIn() const;        // needed by T6 energy report
    double getWhOut() const;       // needed by T6 energy report
    double getEfficiency() const;  // percent, Wh out / Wh in
    double getCycleCount() const;  // equivalent full cycles (can be fractional)
 
private:
    double whIn;
    double whOut;
    double chargeThroughputAh;  // total Ah discharged, used for cycle count
    double capacityAh;          // Ah in one full cycle
};