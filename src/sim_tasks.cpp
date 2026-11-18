#include <iostream>
#include <string>
#include <limits>
#include "tasks.h"
#include "simulator.h"

using namespace std;


// Task 4 - Run discharge cycle
void dischargeCycle(Simulator& sim)
{
    int choice;
    double current;
    double high, low, period;
    string filename;

    cout << "\n===== Discharge Cycle =====" << endl;
    cout << "1. Constant current" << endl;
    cout << "2. Pulsed current" << endl;
    cout << "3. CSV load profile" << endl;
    cout << "0. Return to main menu" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input!" << endl;
        return;
    }

    switch (choice)
    {
        case 1:
        {
            cout << "Enter current (A): ";
            cin >> current;

            if (cin.fail() || current <= 0)
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid current!" << endl;
                return;
            }

            sim.getLoadProfile().setConstant(current);
            cout << "Constant current has been set." << endl;
            break;
        }

        case 2:
        {
            cout << "Enter high current (A): ";
            cin >> high;

            if (cin.fail() || high <= 0)
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid high current!" << endl;
                return;
            }

            cout << "Enter low current (A): ";
            cin >> low;

            if (cin.fail() || low < 0 || low > high)
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid low current!" << endl;
                return;
            }

            cout << "Enter period (seconds): ";
            cin >> period;

            if (cin.fail() || period <= 0)
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid period!" << endl;
                return;
            }

            sim.getLoadProfile().setPulsed(high, low, period);
            cout << "Pulsed current has been set." << endl;
            break;
        }

        case 3:
        {
            cout << "Enter CSV filename: ";
            cin >> filename;

            if (sim.getLoadProfile().loadCSV(filename))
            {
                cout << "CSV file loaded successfully." << endl;
            }
            else
            {
                cout << "Unable to load CSV file." << endl;
                return;
            }

            break;
        }

        case 0:
            return;

        default:
            cout << "Invalid option!" << endl;
            return;
    }

    if (sim.getProtection().isFaulted())
    {
        cout << "Cannot discharge. A fault is active." << endl;
        return;
    }

    cout << "\nStarting discharge simulation..." << endl;
    sim.runDischarge();
}


// Task 8 - View event log
void viewEvents(Simulator& sim)
{
    cout << "\n===== Event Log =====" << endl;

    cout << "Total events: "
         << sim.getEventLog().size() << endl;

    sim.getEventLog().print();
}


// Task 9 - Enable or disable CSV logging
void toggleLog(Simulator& sim)
{
    string filename;

    cout << "\n===== CSV Logging =====" << endl;

    if (sim.getLogger().isEnabled())
    {
        sim.getLogger().disable();
        cout << "CSV logging has been disabled." << endl;
    }
    else
    {
        cout << "Enter filename (0 for default): ";
        cin >> filename;

        if (filename == "0")
        {
            filename = "bms_log.csv";
        }

        if (sim.getLogger().enable(filename))
        {
            cout << "CSV logging enabled." << endl;
            cout << "Filename: " << filename << endl;
        }
        else
        {
            cout << "Unable to enable CSV logging." << endl;
        }
    }
}


// Task 10 - Run demo scenario
void runScenario(Simulator& sim)
{
    cout << "\n===== BMS Demo Scenario =====" << endl;

    if (sim.getProtection().isFaulted())
    {
        cout << "Cannot start demo. A fault is active." << endl;
        return;
    }

    sim.resetTime();

    cout << "\nInitial battery status:" << endl;
    sim.printStatusLine();

    // Set current to 3 A for the demonstration
    sim.getLoadProfile().setConstant(3.0);

    cout << "\nRunning discharge at 3 A..." << endl;
    sim.runDischarge();

    cout << "\nFinal battery status:" << endl;
    sim.printStatusLine();

    cout << "\nEvent Log:" << endl;
    sim.getEventLog().print();

    cout << "\nDemo scenario completed." << endl;
}


// Task 0 - Exit program
void exitProgram(Simulator& sim)
{
    sim.getLogger().disable();

    cout << "Exiting BMS simulator." << endl;
}
