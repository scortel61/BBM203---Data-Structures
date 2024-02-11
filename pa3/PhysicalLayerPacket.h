#ifndef PHYSICAL_LAYER_PACKET_H
#define PHYSICAL_LAYER_PACKET_H

#include "Packet.h"

using namespace std;

// Extends Packet class. Have additional layer-specific member variables and overrides the virtual print function.
class PhysicalLayerPacket : public Packet {
public:
    PhysicalLayerPacket(int layerID, const string &senderMAC, const string &receiverMAC);

    ~PhysicalLayerPacket() override;

    string sender_MAC_address;
    string receiver_MAC_address;
    int hop_count = 0;

    string get_custom_attribute() override;

    void print() override;

    void print2() override;

    void increment_hop() override;

    string get_sender() override;

    string get_receiver() override;

    void set_sender(const string &value) override;

    void set_receiver(const string &value) override;
};

#endif // PHYSICAL_LAYER_PACKET_H
