#include "pack.h"
#include <iostream>

Pack::Pack() : numSeries(0) {
    configure(4, 3.0, 0.02, 1.0);   // default: 4S, 3.0 Ah, 20 mOhm, full
}

void Pack::configure(int numCells, double capacityAh, double resistance, double startSOC) {
    numSeries = numCells;
    cells.assign(numCells, Cell(capacityAh, resistance, startSOC));
}

void Pack::applyCurrent(double current, double dt) {
    for (Cell& c : cells) c.update(current, dt);
}

double Pack::getVoltage() const {
    double total = 0.0;
    for (const Cell& c : cells) total += c.getTerminalVoltage();
    return total;
}

double Pack::getMaxCellVoltage() const {
    // TODO: highest cell terminal voltage
    return cells.empty() ? 0.0 : cells[0].getTerminalVoltage();
}

double Pack::getMinCellVoltage() const {
    // TODO: lowest cell terminal voltage
    return cells.empty() ? 0.0 : cells[0].getTerminalVoltage();
}

double Pack::getMaxTemperature() const {
    // TODO: hottest cell temperature
    return cells.empty() ? 0.0 : cells[0].getTemperature();
}

double Pack::getSOC() const {
    // TODO: average SOC of all cells
    return cells.empty() ? 0.0 : cells[0].getSOC();
}

double Pack::getCapacity() const { return cells.empty() ? 0.0 : cells[0].getCapacity(); }
int Pack::getNumCells() const { return numSeries; }

void Pack::printCellStatus() const {
    // TODO: print voltage, SOC, and temperature for each cell
    std::cout << "Pack: " << numSeries << " cells in series (cell status not implemented yet)\n";
}