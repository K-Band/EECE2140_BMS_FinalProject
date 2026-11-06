#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include "tasks.h"
#include "simulator.h"
#include "pack.h"
#include "charger.h"
#include "soc_estimator.h"
#include "energy_tracker.h"
#include "protection.h"
#include "contactor.h"

using namespace std;

template <typename T>
static bool readNumber(T& value)
{ 
    if (cin >> value) return true;
    if (cin.eof()) return false;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Please enter a number." << endl;
    return false;
}

// T3: run a CC-CV charge and summarize the result
void chargeCycle(Simulator& sim)
{
    Pack& pack = sim.getPack();
    Charger& charger = sim.getCharger();
    EnergyTracker& energy = sim.getEnergyTracker();
    Protection& prot = sim.getProtection();
 
    if (prot.isFaulted()) {
        cout << "Cannot charge: " << prot.getFaultReason() << endl;
        cout << "Use option 7 to reset the fault first." << endl;
        return;
    }
 
    double startSOC = pack.getSOC();
    double startWhIn = energy.getWhIn();
 
    cout << fixed << setprecision(1);
    cout << "Starting CC-CV charge at " << startSOC * 100.0 << "% SOC..." << endl;
    charger.reset();
    sim.runCharge();
 
    cout << "\n--- Charge summary ---" << endl;
    if (prot.isFaulted()) {
        cout << "Stopped by fault: " << prot.getFaultReason() << endl;
    } else if (charger.isDone()) {
        cout << "Charge complete: current tapered below the cutoff." << endl;
    } else {
        cout << "Charge stopped in " << charger.getModeName() << " mode." << endl;
    }
    cout << "SOC: " << startSOC * 100.0 << "% -> " << pack.getSOC() * 100.0 << "%" << endl;
    cout << setprecision(3);
    cout << "Energy added: " << energy.getWhIn() - startWhIn << " Wh" << endl;
}
 
// T5: compare the coulomb-counted estimate with the true SOC
void showSOC(Simulator& sim)
{
    double trueSOC = sim.getPack().getSOC();
    SocEstimator& estimator = sim.getSocEstimator();
 
    cout << fixed << setprecision(2);
    cout << "\n--- SOC estimate vs. true SOC ---" << endl;
    cout << "True SOC:      " << setw(7) << trueSOC * 100.0 << " %" << endl;
    cout << "Estimated SOC: " << setw(7) << estimator.getEstimate() * 100.0 << " %" << endl;
    cout << "Error:         " << setw(7) << estimator.getError(trueSOC) * 100.0 << " %" << endl;
    cout << "(Negative error means the estimate is below the true SOC.)" << endl;
}
 
// T6: energy in/out, efficiency, and cycle count
void reportEnergy(Simulator& sim)
{
    EnergyTracker& energy = sim.getEnergyTracker();
 
    cout << fixed << setprecision(3);
    cout << "\n--- Energy report ---" << endl;
    cout << "Energy in:   " << setw(9) << energy.getWhIn() << " Wh" << endl;
    cout << "Energy out:  " << setw(9) << energy.getWhOut() << " Wh" << endl;
 
    // Efficiency only means something once the pack has been charged AND discharged
    cout << setprecision(1);
    if (energy.getWhIn() <= 0.0) {
        cout << "Efficiency:        n/a (no charging recorded yet)" << endl;
    } else if (energy.getWhOut() <= 0.0) {
        cout << "Efficiency:        n/a (no discharge recorded yet)" << endl;
    } else {
        cout << "Efficiency:  " << setw(9) << energy.getEfficiency() << " %" << endl;
    }
 
    cout << setprecision(2);
    cout << "Cycle count: " << setw(9) << energy.getCycleCount() << " equivalent full cycles" << endl;
}
 
// Prints every limit plus the current fault and warning status (used by T7)
static void printLimits(Protection& prot)
{
    const char* units[Protection::NUM_CHECKS] = {"V", "V", "A", "C", "%"};
 
    cout << fixed << setprecision(2);
    cout << "\n--- Protection limits ---" << endl;
    cout << left << setw(22) << "Check"
         << right << setw(7) << "Warning" << "     " << setw(7) << "Fault" << endl;
    for (int i = 0; i < Protection::NUM_CHECKS; i++) {
        double scale = (i == Protection::LOW_SOC) ? 100.0 : 1.0;  // show SOC as a percent
        double warn = prot.getLimit(i, Protection::WARNING) * scale;
        double fault = prot.getLimit(i, Protection::FAULT) * scale;
        string label = to_string(i + 1) + ". " + Protection::getCheckName(i);
        cout << left << setw(22) << label
             << right << setw(7) << warn << " " << left << setw(4) << units[i]
             << right << setw(7) << fault << " " << units[i] << endl;
    }
    cout << left;
 
    if (prot.isFaulted()) {
        cout << "Status: FAULTED - " << prot.getFaultReason() << endl;
    } else {
        cout << "Status: no fault" << endl;
    }
    if (!prot.getActiveWarnings().empty()) {
        cout << "Active warnings:" << endl;
        for (const string& w : prot.getActiveWarnings()) {
            cout << "  " << w << endl;
        }
    }
}
 
// T7: view and change protection limits, and reset a latched fault
void setThresholds(Simulator& sim)
{
    Protection& prot = sim.getProtection();
 
    while (true) {
        printLimits(prot);
        cout << "\n1. Change a limit\n"
             << "2. Reset latched fault\n"
             << "0. Back to main menu\n"
             << "Select an option: ";
 
        int choice;
        if (!readNumber(choice)) {
            if (cin.eof()) return;
            continue;
        }
 
        switch (choice) {
            case 0:
                return;
 
            case 1: {
                int which, level;
                double value;
                cout << "Which check (1-5)? ";
                if (!readNumber(which)) break;
                if (which < 1 || which > Protection::NUM_CHECKS) {
                    cout << "Invalid check." << endl;
                    break;
                }
                cout << "1 = warning, 2 = fault: ";
                if (!readNumber(level)) break;
                if (level != 1 && level != 2) {
                    cout << "Invalid level." << endl;
                    break;
                }
 
                int index = which - 1;
                bool isSOC = (index == Protection::LOW_SOC);
                cout << "New value" << (isSOC ? " (percent)" : "") << ": ";
                if (!readNumber(value)) break;
                if (isSOC) value /= 100.0;  // stored as a fraction
 
                Protection::Level lvl = (level == 1) ? Protection::WARNING : Protection::FAULT;
                if (!prot.setLimit(index, lvl, value)) {
                    cout << "Invalid value; limit not changed." << endl;
                    break;
                }
                cout << "Limit updated." << endl;
 
                // Warn if the warning level is now past the fault level
                double warn = prot.getLimit(index, Protection::WARNING);
                double fault = prot.getLimit(index, Protection::FAULT);
                bool lowSide = (index == Protection::UNDER_VOLTAGE || index == Protection::LOW_SOC);
                if (lowSide ? warn <= fault : warn >= fault) {
                    cout << "Note: the warning limit is past the fault limit, "
                         << "so this check will fault without warning first." << endl;
                }
                break;
            }
 
            case 2:
                if (!prot.isFaulted()) {
                    cout << "No fault to reset." << endl;
                    break;
                }
                prot.resetFault();
                // Re-check at rest: if the condition is still there, it trips again
                if (prot.check(sim.getPack(), 0.0) == Protection::FAULT) {
                    cout << "Fault condition still present: " << prot.getFaultReason() << endl;
                    cout << "Contactor stays open." << endl;
                } else {
                    sim.getContactor().close();
                    cout << "Fault cleared. Contactor closed." << endl;
                }
                break;
 
            default:
                cout << "Invalid option." << endl;
        }
    }
}