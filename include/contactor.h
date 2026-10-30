// contactor.h - simulated contactor that connects the pack to the load/charger (Drew)
#pragma once
 
class Contactor {
public:
    Contactor();     // starts closed (connected)
 
    void open();     // disconnect load and charger
    void close();    // reconnect
    bool isClosed() const;
 
private:
    bool closed;
};