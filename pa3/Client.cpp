//
// Created by alperen on 27.09.2023.
//

#include "Client.h"

Client::Client(string const &_id, string const &_ip, string const &_mac) {
    client_id = _id;
    client_ip = _ip;
    client_mac = _mac;
}

ostream &operator<<(ostream &os, const Client &client) {
    os << "client_id: " << client.client_id << " client_ip: " << client.client_ip << " client_mac: "
       << client.client_mac << endl;
    return os;
}

Client::~Client() {
    // TODO: Free any dynamically allocated memory if necessary.
}

void Client::printLogEntries() {
    cout << "Client " << client_id << " Logs:" << endl;
    for (int i = 0; i < log_entries.size(); ++i) {
        auto logEntry = log_entries[i];
        cout << "--------------" << endl;
        cout << "Log Entry #" << (i + 1) << ":" << endl;
        string activityString;
        if (logEntry.activity_type == ActivityType::MESSAGE_SENT) {
            activityString = "Message Sent";
        } else if (logEntry.activity_type == ActivityType::MESSAGE_FORWARDED) {
            activityString = "Message Forwarded";
        } else if (logEntry.activity_type == ActivityType::MESSAGE_RECEIVED) {
            activityString = "Message Received";
        } else if (logEntry.activity_type == ActivityType::MESSAGE_DROPPED) {
            activityString = "Message Dropped";
        }
        cout << "Activity: " << activityString << endl;
        cout << "Timestamp: " << logEntry.timestamp << endl;
        cout << "Number of frames: " << logEntry.number_of_frames << endl;
        cout << "Number of hops: " << logEntry.number_of_hops << endl;
        cout << "Sender ID: " << logEntry.sender_id << endl;
        cout << "Receiver ID: " << logEntry.receiver_id << endl;
        cout << "Success: " << (logEntry.success_status ? "Yes" : "No") << endl;
        if (logEntry.activity_type == ActivityType::MESSAGE_SENT ||
            logEntry.activity_type == ActivityType::MESSAGE_RECEIVED) {
            cout << "Message: \"" << logEntry.message_content << "\"" << endl;
        }
    }
}
