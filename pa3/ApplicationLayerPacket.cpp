#include "ApplicationLayerPacket.h"

ApplicationLayerPacket::ApplicationLayerPacket(int _layer_ID, const string &_sender_ID, const string &_receiver_ID,
                                               const string &_message_data) : Packet(_layer_ID) {
    sender_ID = _sender_ID;
    receiver_ID = _receiver_ID;
    message_data = _message_data;
}

void ApplicationLayerPacket::print() {
    // Override the virtual print function from Packet class to additionally print layer-specific properties.
    cout << "Sender ID: " << sender_ID << ", Receiver ID: " << receiver_ID << endl;
}

ApplicationLayerPacket::~ApplicationLayerPacket() {
    // Free any dynamically allocated memory if necessary.
}

void ApplicationLayerPacket::print2() {
    cout << "Carried Message: \"" << message_data << "\"" << endl;
}

string ApplicationLayerPacket::get_relative_packet_index() {
    return to_string(this->relative_packet_index);
}

void ApplicationLayerPacket::print3() {
    cout << "Message chunk carried: \"" << message_data << "\"" << endl;
}

string ApplicationLayerPacket::get_sender() {
    return sender_ID;
}

string ApplicationLayerPacket::get_receiver() {
    return receiver_ID;
}

string ApplicationLayerPacket::get_custom_attribute() {
    return message_data;
}
