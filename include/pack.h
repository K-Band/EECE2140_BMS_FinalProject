#ifndef PACK_H
#define PACK_H

#include <vector>
#include "cell.h"

// Owner: Kavi
// Cells connected in series. The same current flows through every cell,
// and the pack voltage is the sum of the cell terminal voltages.
class Pack {
public:
    Pack();

    void configure(int numCells, double capacityAh, double resistance, double startSOC);
    void applyCurrent(double current, double dt);

    double getVoltage() const;
    double getMaxCellVoltage() const;
    double getMinCellVoltage() const;
    double getMaxTemperature() const;
    double getSOC() const;              // average SOC of all cells
    double getCapacity() const;         // Ah (series pack capacity = cell capacity)
    int getNumCells() const;
    void printCellStatus() const;

private:
    std::vector<Cell> cells;
    int numSeries;
};

#endif