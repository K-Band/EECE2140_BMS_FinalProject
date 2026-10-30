#include "simulator.h"
#include <iostream>

Simulator::Simulator(double timestep) : time(0.0), timestep(timestep), lastCurrent(0.0) {}

void Simulator::runCharge() {
    // TODO (Anthony + Drew): loop step(-charger.getChargeCurrent(pack))
    //                        until charger.isDone() or protection.isFaulted()
    std::cout << "runCharge: not implemented yet.\n";
}

void Simulator::runDischarge() {
    // TODO (Anthony): loop step(loadProfile.getCurrent(time))
    //                 until low SOC, undervoltage, or fault
    std::cout << "runDischarge: not implemented yet.\n";
}

void Simulator::step(double current) {
    // TODO (Anthony): if contactor open, current = 0; update pack, estimator,
    //                 energy, protection (open contactor on fault), log, advance time
    lastCurrent = current;
    time += timestep;
}

void Simulator::printStatusLine() const {
    // TODO (Anthony): time, V, I, ASCII SOC bar, temperature, warnings
    std::cout << "t=" << time << " s  V=" << pack.getVoltage() << " V  I=" << lastCurrent
              << " A  SOC=" << pack.getSOC() * 100.0 << "%\n";
}

void Simulator::resetTime() { time = 0.0; }

double Simulator::getTime() const { return time; }
double Simulator::getTimestep() const { return timestep; }

Pack& Simulator::getPack() { return pack; }
Charger& Simulator::getCharger() { return charger; }
SocEstimator& Simulator::getSocEstimator() { return socEstimator; }
EnergyTracker& Simulator::getEnergyTracker() { return energyTracker; }
Protection& Simulator::getProtection() { return protection; }
Contactor& Simulator::getContactor() { return contactor; }
LoadProfile& Simulator::getLoadProfile() { return loadProfile; }
Logger& Simulator::getLogger() { return logger; }
EventLog& Simulator::getEventLog() { return eventLog; }
