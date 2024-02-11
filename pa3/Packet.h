#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <iostream>

using namespace std;

class Packet {
public:
    Packet(int layerID);

    virtual ~Packet();

    int layer_ID;

    friend ostream &operator<<(ostream &os, const Packet &packet);

    virtual void print() {};

    virtual void print2() {};

    virtual void print3() {};

    virtual void increment_hop() {};

    virtual string get_relative_packet_index() {
        return "";
    }

    virtual string get_custom_attribute() {
        return "";
    };

    virtual string get_sender() {
        return "";
    };

    virtual string get_receiver() {
        return "";
    };

    virtual void set_sender(const string &value) {};

    virtual void set_receiver(const string &value) {};
};

#endif // PACKET_H
