// contactor.cpp - simulated contactor (Drew)
#include "contactor.h"
 
Contactor::Contactor() : closed(true) {}
 
void Contactor::open() {
    closed = false;
}
 
void Contactor::close() {
    closed = true;
}
 
bool Contactor::isClosed() const {
    return closed;
}