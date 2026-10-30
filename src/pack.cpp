#include "pack.h"
#include <iostream>
#include <iomanip>

// Creates a default 4-cell, 3.0 Ah, 20 mOhm pack at full charge.
Pack::Pack() : numSeries(0) {
    configure(4, 3.0, 0.02, 1.0);
}

// Rebuilds the pack with the given number of identical cells.
void Pack::configure(int numCells, double capacityAh, double resistance, double startSOC) {
    if (numCells < 1) numCells = 1;
    numSeries = numCells;
    cells.assign(numCells, Cell(capacityAh, resistance, startSOC));
}

// Applies the same current to every cell for one time step.
void Pack::applyCurrent(double current, double dt) {
    for (Cell& c : cells) c.update(current, dt);
}

// Returns the total pack voltage as the sum of all cell voltages.
double Pack::getVoltage() const {
    double total = 0.0;
    for (const Cell& c : cells) total += c.getTerminalVoltage();
    return total;
}

// Returns the highest cell voltage in the pack.
double Pack::getMaxCellVoltage() const {
    if (cells.empty()) return 0.0;
    double maxV = cells[0].getTerminalVoltage();
    for (const Cell& c : cells) {
        if (c.getTerminalVoltage() > maxV) maxV = c.getTerminalVoltage();
    }
    return maxV;
}

// Returns the lowest cell voltage in the pack.
double Pack::getMinCellVoltage() const {
    if (cells.empty()) return 0.0;
    double minV = cells[0].getTerminalVoltage();
    for (const Cell& c : cells) {
        if (c.getTerminalVoltage() < minV) minV = c.getTerminalVoltage();
    }
    return minV;
}

// Returns the temperature of the hottest cell.
double Pack::getMaxTemperature() const {
    if (cells.empty()) return 0.0;
    double maxT = cells[0].getTemperature();
    for (const Cell& c : cells) {
        if (c.getTemperature() > maxT) maxT = c.getTemperature();
    }
    return maxT;
}

// Returns the average SOC of all cells.
double Pack::getSOC() const {
    if (cells.empty()) return 0.0;
    double total = 0.0;
    for (const Cell& c : cells) total += c.getSOC();
    return total / cells.size();
}

// Returns the pack capacity in amp-hours.
double Pack::getCapacity() const { return cells.empty() ? 0.0 : cells[0].getCapacity(); }

// Returns the number of cells in series.
int Pack::getNumCells() const { return numSeries; }

// Prints a table of each cell's voltage, SOC, and temperature.
void Pack::printCellStatus() const {
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Cell   Voltage (V)   SOC (%)   Temp (C)\n";
    for (size_t i = 0; i < cells.size(); i++) {
        std::cout << std::setw(4)  << i + 1
                  << std::setw(14) << cells[i].getTerminalVoltage()
                  << std::setw(10) << std::setprecision(1) << cells[i].getSOC() * 100.0
                  << std::setw(11) << cells[i].getTemperature()
                  << std::setprecision(3) << "\n";
    }
    std::cout << "Pack: " << getVoltage() << " V, "
              << std::setprecision(1) << getSOC() * 100.0 << "% SOC, max "
              << getMaxTemperature() << " C\n";
}