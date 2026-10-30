#include <iostream>
#include "tasks.h"
#include "simulator.h"

using namespace std;

void dischargeCycle(Simulator& sim)
{
    cout << "Discharge cycle not implemented yet." << endl;
}

void viewEvents(Simulator& sim)
{
    cout << "View event log not implemented yet." << endl;
}

void toggleLog(Simulator& sim)
{
    cout << "Toggle CSV logging not implemented yet." << endl;
}

void runScenario(Simulator& sim)
{
    cout << "Demo scenario not implemented yet." << endl;
}

void exitProgram(Simulator& sim)
{
    sim.getLogger().disable();
    cout << "Exiting BMS simulator." << endl;
}