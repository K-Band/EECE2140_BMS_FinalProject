#include "ocv_table.h"

// Typical NMC OCV curve at 25 deg C (placeholder).
// TODO: replace with HG2 values from the McMaster C/20 discharge test.
OcvTable::OcvTable()
    : socPoints{0.00, 0.05, 0.10, 0.20, 0.30, 0.40, 0.50,
                0.60, 0.70, 0.80, 0.90, 0.95, 1.00},
      
        ocvValues{3.00, 3.30, 3.45, 3.55, 3.61, 3.66, 3.72,
                3.80, 3.88, 3.97, 4.06, 4.12, 4.20} {}

// finds the OCV at any point using linear interpolation of the table 
double OcvTable::lookup(double soc) const {
    
    if (soc <= socPoints.front()) return ocvValues.front();
    if (soc >= socPoints.back()) return ocvValues.back();

    // Find the first table point above soc, then interpolate
    // between it and the point just before it
    for (size_t i = 1; i < socPoints.size(); i++) {
        if (soc <= socPoints[i]) {
            double s1 = socPoints[i - 1];
            double s2 = socPoints[i];
            double v1 = ocvValues[i - 1];
            double v2 = ocvValues[i];
            return v1 + (soc - s1) * (v2 - v1) / (s2 - s1);
        }
    }

    return ocvValues.back();   
}

double OcvTable::getMinVoltage() const { return ocvValues.front(); }
double OcvTable::getMaxVoltage() const { return ocvValues.back(); }