#include "PhysicalLayerPacket.h"

PhysicalLayerPacket::PhysicalLayerPacket(int _layer_ID, const string &_sender_MAC, const string &_receiver_MAC)
        : Packet(_layer_ID) {
    sender_MAC_address = _sender_MAC;
    receiver_MAC_address = _receiver_MAC;
}

void PhysicalLayerPacket::print() {
    // Override the virtual print function from Packet class to additionally print layer-specific properties.
    cout << "Sender MAC address: " << sender_MAC_address << ", Receiver MAC address: "
         << receiver_MAC_address << endl;
}

PhysicalLayerPacket::~PhysicalLayerPacket() {
    // Free any dynamically allocated memory if necessary.
}

void PhysicalLayerPacket::print2() {
    cout << "Number of hops so far: " << hop_count << endl;
}

void PhysicalLayerPacket::increment_hop() {
    this->hop_count++;
}

string PhysicalLayerPacket::get_sender() {
    return sender_MAC_address;
}

string PhysicalLayerPacket::get_receiver() {
    return receiver_MAC_address;
}

string PhysicalLayerPacket::get_custom_attribute() {
    return to_string(hop_count);
}

void PhysicalLayerPacket::set_sender(const string &value) {
    this->sender_MAC_address = value;
}

void PhysicalLayerPacket::set_receiver(const string &value) {
    this->receiver_MAC_address = value;
}
