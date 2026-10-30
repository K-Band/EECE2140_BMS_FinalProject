#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "pack.h"
#include "charger.h"
#include "soc_estimator.h"
#include "energy_tracker.h"
#include "protection.h"
#include "contactor.h"
#include "load_profile.h"
#include "logger.h"
#include "event_log.h"

// Owner: Anthony
// Owns one of every subsystem and runs the fixed time-step loop.
// Each step: get current -> check contactor -> update pack -> update estimator
//            -> update energy -> check protection -> log.
class Simulator {
public:
    Simulator(double timestep = 1.0);

    void runCharge();
    void runDischarge();
    void step(double current);        // advance the whole system by one time step
    void printStatusLine() const;
    void resetTime();

    double getTime() const;
    double getTimestep() const;

    Pack& getPack();
    Charger& getCharger();
    SocEstimator& getSocEstimator();
    EnergyTracker& getEnergyTracker();
    Protection& getProtection();
    Contactor& getContactor();
    LoadProfile& getLoadProfile();
    Logger& getLogger();
    EventLog& getEventLog();

private:
    Pack pack;
    Charger charger;
    SocEstimator socEstimator;
    EnergyTracker energyTracker;
    Protection protection;
    Contactor contactor;
    LoadProfile loadProfile;
    Logger logger;
    EventLog eventLog;
    double time;       // s
    double timestep;   // s
    double lastCurrent;
};

#endif