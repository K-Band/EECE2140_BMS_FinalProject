// Owner: Drew - tests for Charger, SocEstimator, EnergyTracker, Protection, Contactor
#include <cassert>
#include <cmath>
#include <iostream>
#include "pack.h"
#include "charger.h"
#include "soc_estimator.h"
#include "energy_tracker.h"
#include "protection.h"
#include "contactor.h"
 
int main() {
    //Charger tests
 
    // CC-CV: starts in CC, switches to CV, then finishes with zero current
    Pack pack;
    pack.configure(4, 3.0, 0.02, 0.2);
    Charger charger(3.0, 4.2, 0.15);
    assert(charger.getMode() == Charger::CC);
    assert(charger.getModeName() == "CC");
 
    bool sawCV = false;
    double maxCellV = 0.0;
    int steps = 0;
    while (!charger.isDone() && steps < 20000) {
        double current = charger.getChargeCurrent(pack);
        assert(current <= 0.0);  // the charger never discharges the pack
        if (charger.getMode() == Charger::CV) sawCV = true;
        pack.applyCurrent(current, 1.0);
        if (pack.getMaxCellVoltage() > maxCellV) maxCellV = pack.getMaxCellVoltage();
        steps++;
    }
    assert(sawCV);
    assert(charger.isDone());
    assert(maxCellV < 4.21);         // CV holds the highest cell at about 4.2 V
    assert(pack.getSOC() > 0.95);    // pack ends nearly full
    assert(charger.getChargeCurrent(pack) == 0.0);  // done means no more current
 
    // reset() puts the charger back in CC for the next charge
    charger.reset();
    assert(charger.getMode() == Charger::CC);
 
    //SOC estimator tests
 
    // With no noise or offset, coulomb counting is exact:
    // 3 A for 1800 s on a 3 Ah cell removes 0.5 (50%)
    SocEstimator exact(3.0, 0.0, 0.0);
    exact.reset(1.0);
    for (int i = 0; i < 1800; i++) exact.update(3.0, 1.0);
    assert(std::fabs(exact.getEstimate() - 0.5) < 1e-9);
 
    // getError is the estimate minus the true SOC
    assert(std::fabs(exact.getError(0.4) - 0.1) < 1e-9);
 
    // Charging (negative current) raises the estimate and it never goes above 1
    for (int i = 0; i < 10000; i++) exact.update(-3.0, 1.0);
    assert(exact.getEstimate() == 1.0);
 
    // A 0.05 A sensor offset at rest drifts the estimate down by 0.05 Ah / 3 Ah per hour
    SocEstimator biased(3.0, 0.0, 0.05);
    biased.reset(0.5);
    for (int i = 0; i < 3600; i++) biased.update(0.0, 1.0);
    assert(std::fabs(biased.getEstimate() - (0.5 - 0.05 / 3.0)) < 1e-9);
 
    // Noise alone mostly averages out over a 1-hour discharge
    SocEstimator noisy(3.0, 0.05, 0.0);
    noisy.reset(1.0);
    for (int i = 0; i < 3600; i++) noisy.update(1.5, 1.0);
    assert(std::fabs(noisy.getEstimate() - 0.5) < 0.01);
 
    //Energy tracker tests
 
    // 4 V at 3 A for 1 hour = 12 Wh out, and one full cycle on a 3 Ah cell
    EnergyTracker energy(3.0);
    energy.update(4.0, 3.0, 3600.0);
    assert(std::fabs(energy.getWhOut() - 12.0) < 1e-9);
    assert(std::fabs(energy.getCycleCount() - 1.0) < 1e-9);
    assert(energy.getWhIn() == 0.0);
    assert(energy.getEfficiency() == 0.0);  // nothing charged in yet
 
    // Charging only adds to Wh in: 15 Wh in gives 12 / 15 = 80% efficiency
    energy.update(5.0, -3.0, 3600.0);
    assert(std::fabs(energy.getWhIn() - 15.0) < 1e-9);
    assert(std::fabs(energy.getEfficiency() - 80.0) < 1e-9);
    assert(std::fabs(energy.getCycleCount() - 1.0) < 1e-9);  // charging adds no cycles
 
    //Protection tests
 
    Pack normal;
    normal.configure(4, 3.0, 0.02, 0.5);
    Protection prot;
 
    // setLimit / getLimit, including rejecting bad input
    assert(prot.setLimit(Protection::OVER_CURRENT, Protection::WARNING, 5.0));
    assert(prot.setLimit(Protection::OVER_CURRENT, Protection::FAULT, 7.5));
    assert(prot.getLimit(Protection::OVER_CURRENT, Protection::FAULT) == 7.5);
    assert(!prot.setLimit(99, Protection::FAULT, 1.0));                     // invalid check
    assert(!prot.setLimit(Protection::OVER_CURRENT, Protection::OK, 1.0));  // invalid level
    assert(Protection::getCheckName(Protection::OVER_TEMP) == "Overtemperature");
 
    // Normal conditions report OK
    assert(prot.check(normal, 1.0) == Protection::OK);
    assert(!prot.isFaulted());
 
    // Between the warning and fault limits: WARNING, but no fault
    assert(prot.check(normal, 6.0) == Protection::WARNING);
    assert(prot.getActiveWarnings().size() == 1);
    assert(!prot.isFaulted());
 
    // Fault latching: an overcurrent fault stays latched after the current drops
    assert(prot.check(normal, 8.0) == Protection::FAULT);
    assert(prot.isFaulted());
    assert(!prot.getFaultReason().empty());
    assert(prot.check(normal, 0.0) == Protection::FAULT);
 
    // resetFault clears the latch once the condition is gone
    prot.resetFault();
    assert(prot.check(normal, 0.0) == Protection::OK);
 
    // If the condition is still there, it faults again on the next check
    prot.check(normal, 8.0);
    prot.resetFault();
    assert(prot.check(normal, 8.0) == Protection::FAULT);
 
    // Low SOC faults while discharging but not while charging,
    // so a nearly empty pack can still be recharged
    Pack empty;
    empty.configure(4, 3.0, 0.02, 0.03);
    Protection prot2;
    assert(prot2.check(empty, -1.0) == Protection::OK);
    assert(prot2.check(empty, 1.0) == Protection::FAULT);
    assert(prot2.getFaultReason().find("Low SOC") != std::string::npos);
 
    //Contactor tests
 
    // Starts closed, then opens and closes on command
    Contactor contactor;
    assert(contactor.isClosed());
    contactor.open();
    assert(!contactor.isClosed());
    contactor.close();
    assert(contactor.isClosed());
 
    std::cout << "test_control: all tests passed\n";
    return 0;
}