#ifndef APPLICATION_LAYER_PACKET_H
#define APPLICATION_LAYER_PACKET_H

#include "Packet.h"

// Extends Packet class. Have additional layer-specific member variables and overrides the virtual print function.
class ApplicationLayerPacket : public Packet {
public:
    ApplicationLayerPacket(int _layer_ID, const string& _sender_ID, const string& _receiver_ID, const string& _message_data);
    ~ApplicationLayerPacket() override;

    string sender_ID;
    string receiver_ID;
    string message_data;
    int relative_packet_index = 0;

    string get_custom_attribute() override;
    string get_relative_packet_index() override;
    void print() override;
    void print2() override;
    void print3() override;
    string get_sender() override;
    string get_receiver() override;
};

#endif // APPLICATION_LAYER_PACKET_H
