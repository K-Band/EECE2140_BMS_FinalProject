#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdio>
#include "simulator.h"

using namespace std;

int passed = 0;
int failed = 0;


// Check whether a test passed or failed
void check(bool condition, string name)
{
    if (condition)
    {
        cout << "PASS: " << name << endl;
        passed++;
    }
    else
    {
        cout << "FAIL: " << name << endl;
        failed++;
    }
}


// Compare decimal values
bool closeEnough(double a, double b)
{
    return fabs(a - b) < 0.0001;
}


int main()
{
    cout << "===== BMS SIMULATION TESTS =====" << endl;

    // Test constant-current load profile
    LoadProfile load;
    load.setConstant(3.0);

    check(closeEnough(load.getCurrent(0), 3.0), "Constant current at 0 s");
    check(closeEnough(load.getCurrent(20), 3.0), "Constant current at 20 s");

    // Test pulsed-current load profile
    load.setPulsed(6.0, 1.0, 20.0);

    check(closeEnough(load.getCurrent(0), 6.0), "Pulsed high current");
    check(closeEnough(load.getCurrent(10), 1.0), "Pulsed low current");
    check(closeEnough(load.getCurrent(20), 6.0), "Pulsed current repeats");

    // Test CSV load profile
    ofstream testFile("test_sim_load.csv");
    testFile << "time,current\n";
    testFile << "0,3\n";
    testFile << "10,5\n";
    testFile << "20,2\n";
    testFile << "30,0\n";
    testFile.close();

    check(load.loadCSV("test_sim_load.csv"), "Load CSV file");
    check(closeEnough(load.getCurrent(5), 3.0), "CSV current at 5 s");
    check(closeEnough(load.getCurrent(15), 5.0), "CSV current at 15 s");
    check(closeEnough(load.getCurrent(25), 2.0), "CSV current at 25 s");
    check(closeEnough(load.getCurrent(30), 0.0), "CSV stops at 30 s");
    check(!load.loadCSV("missing_test_sim.csv"), "Reject missing CSV file");

    remove("test_sim_load.csv");

    // Test one simulation step
    Simulator sim;

    double startingSOC = sim.getPack().getSOC();
    double startingVoltage = sim.getPack().getVoltage();
    double startingTemp = sim.getPack().getMaxTemperature();

    check(sim.getContactor().isClosed(), "Contactor starts closed");

    sim.step(3.0);

    check(closeEnough(sim.getTime(), sim.getTimestep()), "Simulation time increases");
    check(sim.getPack().getSOC() < startingSOC, "SOC decreases while discharging");
    check(sim.getPack().getVoltage() < startingVoltage, "Voltage decreases while discharging");
    check(sim.getEnergyTracker().getWhOut() > 0, "Discharged energy increases");

    for (int i = 0; i < 59; i++)
    {
        sim.step(3.0);
    }

    check(sim.getPack().getMaxTemperature() > startingTemp, "Battery temperature increases");
    check(closeEnough(sim.getTime(), 60.0), "60 simulation steps take 60 s");

    // Test CSV output logging
    Simulator logSim;

    check(logSim.getLogger().enable("test_sim_output.csv"), "Enable CSV logging");
    check(logSim.getLogger().isEnabled(), "Logger reports enabled");

    logSim.step(3.0);
    logSim.getLogger().disable();

    check(!logSim.getLogger().isEnabled(), "Disable CSV logging");

    ifstream logFile("test_sim_output.csv");
    string header, data;

    getline(logFile, header);
    getline(logFile, data);

    check(header.find("Time") != string::npos, "CSV contains column headers");
    check(!data.empty(), "CSV contains simulation data");

    logFile.close();
    remove("test_sim_output.csv");

    // Test event log
    Simulator eventSim;

    eventSim.getEventLog().add(0.0, "Test event");
    check(eventSim.getEventLog().size() == 1, "Event log stores an event");

    eventSim.getEventLog().clear();
    check(eventSim.getEventLog().size() == 0, "Event log clears events");

    // Test low-SOC protection and contactor shutdown
    Simulator faultSim;

    faultSim.getPack().configure(4, 3.0, 0.02, 0.03);
    faultSim.step(3.0);

    check(faultSim.getProtection().isFaulted(), "Low SOC triggers a fault");
    check(!faultSim.getContactor().isClosed(), "Fault opens the contactor");
    check(faultSim.getEventLog().size() > 0, "Fault is saved in event log");

    // Test full discharge cycle
    Simulator fullSim;

    fullSim.getPack().configure(4, 0.5, 0.02, 0.20);
    fullSim.getLoadProfile().setConstant(3.0);

    double initialSOC = fullSim.getPack().getSOC();

    cout << "\n===== FULL DISCHARGE TEST =====" << endl;

    fullSim.runDischarge();

    check(fullSim.getTime() > 0, "Discharge advances simulation time");
    check(fullSim.getPack().getSOC() < initialSOC, "Discharge reduces SOC");
    check(fullSim.getEventLog().size() >= 2, "Discharge records start and end events");


    // Test charging current
    Simulator chargeStep;

    chargeStep.getPack().configure(4, 3.0, 0.02, 0.20);
    chargeStep.getSocEstimator().reset(0.20);
    chargeStep.getCharger().reset();

    double chargeSOC = chargeStep.getPack().getSOC();
    double chargeVoltage = chargeStep.getPack().getVoltage();
    double chargeCurrent = chargeStep.getCharger().getChargeCurrent(chargeStep.getPack());

    check(chargeCurrent < 0, "Charger produces negative current");

    check(chargeStep.getCharger().getMode() == Charger::CC, "Charger starts in CC mode");

    chargeStep.step(chargeCurrent);

    check(chargeStep.getPack().getSOC() > chargeSOC, "Charging increases SOC");
    check(chargeStep.getPack().getVoltage() > chargeVoltage, "Charging increases voltage");
    check(chargeStep.getEnergyTracker().getWhIn() > 0, "Charging adds energy into pack");
    check(!chargeStep.getProtection().isFaulted(), "Normal charge step does not fault");


    // Test CC-CV charger mode
    Charger modeCharger;
    Pack fullPack;

    fullPack.configure(4, 3.0, 0.02, 1.0);

    modeCharger.getChargeCurrent(fullPack);

    check(modeCharger.getMode() != Charger::CC, "Charger leaves CC at CV target");

    modeCharger.reset();

    check(modeCharger.getMode() == Charger::CC, "Charger reset returns to CC");


    // Test full charge cycle
    Simulator fullCharge;

    fullCharge.getPack().configure(4, 0.5, 0.02, 0.20);
    fullCharge.getSocEstimator().reset(0.20);
    fullCharge.getCharger().reset();

    double initialChargeSOC = fullCharge.getPack().getSOC();

    cout << "\n===== FULL CHARGE TEST =====" << endl;

    fullCharge.runCharge();

    check(fullCharge.getTime() > 0, "Charge cycle advances simulation time");
    check(fullCharge.getPack().getSOC() > initialChargeSOC, "Charge cycle increases SOC");
    check(fullCharge.getEnergyTracker().getWhIn() > 0, "Charge cycle records incoming energy");
    check(fullCharge.getEventLog().size() >= 2, "Charge records start and end events");


    // Test charging when contactor is open
    Simulator blockedCharge;

    blockedCharge.getPack().configure(4, 3.0, 0.02, 0.20);
    blockedCharge.getContactor().open();

    blockedCharge.runCharge();

    check(closeEnough(blockedCharge.getTime(), 0), "Open contactor blocks charging");


    // Display test results
    cout << "\n===== TEST SUMMARY =====" << endl;
    cout << "Passed: " << passed << endl;
    cout << "Failed: " << failed << endl;

    if (failed == 0)
    {
        cout << "All simulation tests passed!" << endl;
        return 0;
    }

    return 1;
}
