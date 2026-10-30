// soc_estimator.h - coulomb-counting SOC estimator (Drew)
#pragma once
#include <random>
 
// SOC is a fraction from 0.0 (empty) to 1.0 (full).
// Positive current = discharge, negative = charge.
 
class SocEstimator {
public:
    SocEstimator(double capacityAh = 3.0, double noiseAmps = 0.05,
                 double offsetAmps = 0.02, unsigned int seed = 42);
 
    // Adds sensor noise and offset to the true current, then counts coulombs.
    void update(double trueCurrent, double dt);
 
    double getEstimate() const;
    double getError(double trueSOC) const;  // estimate minus true (fraction)
    void reset(double startSOC = 1.0);      // start the estimate at the pack's SOC
 
private:
    double estimatedSOC;
    double capacityAh;
    double noiseLevel;    // A, standard deviation of the sensor noise
    double sensorOffset;  // A, constant sensor bias
    std::mt19937 rng;     // fixed seed so runs are repeatable
};