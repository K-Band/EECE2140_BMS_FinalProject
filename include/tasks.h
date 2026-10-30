#ifndef TASKS_H
#define TASKS_H

class Simulator;

// Kavi
void configurePack(Simulator& sim);
void showThermal(Simulator& sim);

// Drew
void chargeCycle(Simulator& sim);
void showSOC(Simulator& sim);
void reportEnergy(Simulator& sim);
void setThresholds(Simulator& sim);

// Anthony
void dischargeCycle(Simulator& sim);
void viewEvents(Simulator& sim);
void toggleLog(Simulator& sim);
void runScenario(Simulator& sim);
void exitProgram(Simulator& sim);

#endif