// soc_estimator.cpp - coulomb-counting SOC estimator (Drew)
#include "soc_estimator.h"
 
SocEstimator::SocEstimator(double capAh, double noiseAmps, double offsetAmps, unsigned int seed)
    : estimatedSOC(1.0),
      capacityAh(capAh),
      noiseLevel(noiseAmps),
      sensorOffset(offsetAmps),
      rng(seed) {}
 
void SocEstimator::update(double trueCurrent, double dt) {
    if (capacityAh <= 0.0) {
        return;
    }
 
    // Simulated current sensor: true value + fixed offset + random noise
    double noise = 0.0;
    if (noiseLevel > 0.0) {
        std::normal_distribution<double> dist(0.0, noiseLevel);
        noise = dist(rng);
    }
    double measuredCurrent = trueCurrent + sensorOffset + noise;
 
    // Coulomb counting: I * dt is amp-seconds; divide by 3600 for amp-hours
    double deltaAh = measuredCurrent * dt / 3600.0;
    estimatedSOC -= deltaAh / capacityAh;  // discharge (positive) lowers SOC
 
    if (estimatedSOC > 1.0) estimatedSOC = 1.0;
    if (estimatedSOC < 0.0) estimatedSOC = 0.0;
}
 
double SocEstimator::getEstimate() const {
    return estimatedSOC;
}
 
double SocEstimator::getError(double trueSOC) const {
    return estimatedSOC - trueSOC;
}
 
void SocEstimator::reset(double startSOC) {
    if (startSOC > 1.0) startSOC = 1.0;
    if (startSOC < 0.0) startSOC = 0.0;
    estimatedSOC = startSOC;
}