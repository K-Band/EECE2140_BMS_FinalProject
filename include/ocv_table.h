#ifndef OCV_TABLE_H
#define OCV_TABLE_H

#include <vector>

// Owner: Kavi
// Open-circuit voltage (OCV) vs. state of charge (SOC) lookup table for one cell.
// SOC is a fraction from 0.0 (empty) to 1.0 (full).
class OcvTable {
public:
    OcvTable();

    double lookup(double soc) const;   // OCV in volts at the given SOC (interpolated)
    double getMinVoltage() const;      // OCV at 0% SOC
    double getMaxVoltage() const;      // OCV at 100% SOC

private:
    std::vector<double> socPoints;     // ascending SOC points, 0.0 to 1.0
    std::vector<double> ocvValues;     // OCV (V) matching each SOC point
};

#endif