#include "ocv_table.h"

// TODO: replace with the OCV-SOC table for the chosen cell chemistry.
// Placeholder: two points, so lookup() is a straight line from 3.0 V to 4.2 V.
OcvTable::OcvTable()
    : socPoints{0.0, 1.0}, ocvValues{3.0, 4.2} {}

double OcvTable::lookup(double soc) const {
    // TODO: linear interpolation between table points
    if (soc <= socPoints.front()) return ocvValues.front();
    if (soc >= socPoints.back()) return ocvValues.back();
    return ocvValues.front() + soc * (ocvValues.back() - ocvValues.front());
}

double OcvTable::getMinVoltage() const { return ocvValues.front(); }
double OcvTable::getMaxVoltage() const { return ocvValues.back(); }