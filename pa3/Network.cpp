#include "Network.h"


Network::Network() {

}

vector<string> splitCustom(const string &input, char delimiter) {
    vector<string> result;
    size_t start = 0;
    size_t end = input.find(delimiter);

    while (end != string::npos) {
        if (input[start] == '#') {
            end = input.rfind('#');
            result.push_back(input.substr(start + 1, end - start - 1));
            start = end + 1;
        } else {
            result.push_back(input.substr(start, end - start));
            start = end + 1;
        }

        end = input.find(delimiter, start);
    }

    result.push_back(input.substr(start));

    return result;
}

string generateTimestamp() {
    // Get the current time
    time_t t = time(nullptr);
    tm tm = *localtime(&t);

    // Format the timestamp as a string
    ostringstream oss;
    oss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
    string timestamp = oss.str();

    return timestamp;
}

void printDashes(size_t dashCount, bool newLine = true) {
    stringstream stringStream;
    for (size_t i = 0; i < dashCount; ++i) {
        stringStream << "-";
    }
    cout << stringStream.str() << (newLine ? "\n" : "") << flush;
}

vector<string> sliceMessage(const string &message, const int &message_limit) {
    vector<string> slicedMessage;

    for (size_t i = 0; i < message.length(); i += message_limit) {
        slicedMessage.push_back(message.substr(i, message_limit));
    }

    return slicedMessage;
}

int findClientIndexWithMAC(vector<Client> &clients, const string &mac) {
    for (int i = 0; i < clients.size(); ++i) {
        if (clients[i].client_mac == mac) {
            return i;
        }
    }
    return -1;
}

int findClientIndexWithClientId(vector<Client> &clients, const string &client_id) {
    for (int i = 0; i < clients.size(); ++i) {
        if (clients[i].client_id == client_id) {
            return i;
        }
    }
    return -1;
}

int findNextHopClientIndex(vector<Client> &clients, const string &current_client_id, const string &receiver_id) {
    auto currentClientIndex = findClientIndexWithClientId(clients, current_client_id);
    auto &currentClient = clients[currentClientIndex];
    auto routes = currentClient.routing_table;
    for (const auto &route: routes) {
        if (route.first == receiver_id) {
            return findClientIndexWithClientId(clients, route.second);
        }
    }
    return -1;
}

void
handleMessage(vector<Client> &clients, const string &sender_port, const string &receiver_port, const string &sender_id,
              const string &receiver_id, const string &message, const int &message_limit) {
    vector<string> messageSliced = sliceMessage(message, message_limit);

    string timestamp = generateTimestamp();
    Log logEntry(
            timestamp,
            message,
            messageSliced.size(),
            0,
            sender_id,
            receiver_id,
            true,
            ActivityType::MESSAGE_SENT
    );

    auto clientIndex = findClientIndexWithClientId(clients, sender_id);
    if (clientIndex < 0) {
        // may there be an error message here
        return;
    }
    Client &sender = clients[clientIndex];

    clientIndex = findClientIndexWithClientId(clients, receiver_id);
    if (clientIndex < 0) {
        // may there be an error message here
        return;
    }
    Client &receiver = clients[clientIndex];

    clientIndex = findNextHopClientIndex(clients, sender_id, receiver_id);
    if (clientIndex < 0) {
        // packet drop
        logEntry.activity_type = ActivityType::MESSAGE_DROPPED;
        logEntry.success_status = false;

        sender.log_entries.push_back(logEntry);
        return;
    }
    cout << "Message to be sent: \"" << message << "\"" << endl << endl;
    for (int i = 0; i < messageSliced.size(); ++i) {
        stack<Packet *> frame;

        auto *applicationLayerPacket = new ApplicationLayerPacket(0, sender_id, receiver_id, messageSliced[i]);
        applicationLayerPacket->relative_packet_index = i + 1;
        frame.push(applicationLayerPacket);

        auto *transportLayerPacket = new TransportLayerPacket(1, sender_port, receiver_port);
        frame.push(transportLayerPacket);

        auto *networkLayerPacket = new NetworkLayerPacket(2, sender.client_ip, receiver.client_ip);
        frame.push(networkLayerPacket);

        auto *physicalLayerPacket = new PhysicalLayerPacket(3, sender.client_mac, clients[clientIndex].client_mac);
        physicalLayerPacket->hop_count = 0;
        frame.push(physicalLayerPacket);

        sender.outgoing_queue.push(frame);

        cout << "Frame #" << (i + 1) << endl;
        physicalLayerPacket->print();
        networkLayerPacket->print();
        transportLayerPacket->print();
        applicationLayerPacket->print();
        applicationLayerPacket->print2();
        physicalLayerPacket->print2();
        printDashes(8);
    }

    sender.log_entries.push_back(logEntry);
}

void handleShowFrameInfo(vector<Client> &clients, const string &client_id, const string &queue_type,
                         const string &frame_number) {
    auto clientIndex = findClientIndexWithClientId(clients, client_id);
    auto &client = clients[clientIndex];
    auto &queueToPrint = queue_type == "in" ? client.incoming_queue : client.outgoing_queue;
    int frameIndex = stoi(frame_number);
    if (queueToPrint.size() < frameIndex) {
        cout << "No such frame." << endl;
        return;
    }
    stack<Packet *> targetFrame;
    for (int i = 0; i < queueToPrint.size(); ++i) {
        auto popped = queueToPrint.front();
        queueToPrint.pop();

        if (i + 1 == frameIndex) {
            targetFrame = popped;
        }

        queueToPrint.push(popped);
    }

    auto physicalLayerPacket = targetFrame.top();
    targetFrame.pop();
    auto networkLayerPacket = targetFrame.top();
    targetFrame.pop();
    auto transportLayerPacket = targetFrame.top();
    targetFrame.pop();
    auto applicationLayerPacket = targetFrame.top();
    targetFrame.pop();

    cout << "Current Frame #" << frame_number << " on the " << (queue_type == "in" ? "incoming" : "outgoing")
         << " queue of client " << client_id << endl;
    applicationLayerPacket->print2();
    cout << "Layer 0 info: ";
    applicationLayerPacket->print();
    cout << "Layer 1 info: ";
    transportLayerPacket->print();
    cout << "Layer 2 info: ";
    networkLayerPacket->print();
    cout << "Layer 3 info: ";
    physicalLayerPacket->print();
    physicalLayerPacket->print2();

    targetFrame.push(applicationLayerPacket);
    targetFrame.push(transportLayerPacket);
    targetFrame.push(networkLayerPacket);
    targetFrame.push(physicalLayerPacket);
}

void handleShowQueueInfo(vector<Client> &clients, const string &client_id, const string &queue_type) {
    auto clientIndex = findClientIndexWithClientId(clients, client_id);
    auto &client = clients[clientIndex];
    auto &queueToPrint = queue_type == "in" ? client.incoming_queue : client.outgoing_queue;

    cout << "Client " << client_id << " " << (queue_type == "in" ? "Incoming" : "Outgoing") << " Queue Status" << endl;
    cout << "Current total number of frames: " << queueToPrint.size() << endl;
}

void handleSend(vector<Client> &clients) {
    for (auto &client: clients) {
        while (!client.outgoing_queue.empty()) {
            auto packetStack = client.outgoing_queue.front();
            client.outgoing_queue.pop();

            auto physicalLayerPacket = packetStack.top();
            packetStack.pop();
            auto networkLayerPacket = packetStack.top();
            packetStack.pop();
            auto transportLayerPacket = packetStack.top();
            packetStack.pop();
            auto applicationLayerPacket = packetStack.top();
            packetStack.pop();

            auto receiverClientIndex = findClientIndexWithMAC(clients, physicalLayerPacket->get_receiver());
            if (receiverClientIndex < 0) {
                // todo drop implement
                continue;
            }
            auto &receiverClient = clients[receiverClientIndex];

            cout << "Client " << client.client_id << " sending frame #"
                 << applicationLayerPacket->get_relative_packet_index() << " to client " << receiverClient.client_id
                 << endl;
            physicalLayerPacket->print();
            networkLayerPacket->print();
            transportLayerPacket->print();
            applicationLayerPacket->print();
            applicationLayerPacket->print3();
            physicalLayerPacket->increment_hop();
            physicalLayerPacket->print2();
            printDashes(8);

            packetStack.push(applicationLayerPacket);
            packetStack.push(transportLayerPacket);
            packetStack.push(networkLayerPacket);
            packetStack.push(physicalLayerPacket);

            receiverClient.incoming_queue.push(packetStack);
        }
    }
}

void handleReceive(vector<Client> &clients) {
    // todo
    for (auto &client: clients) {
        int current_relative, last_relative = 1;
        bool firstPacket = false;
        auto timestamp = generateTimestamp();
        Log log1(
                timestamp,
                "",
                0,
                0,
                "",
                "",
                true,
                ActivityType::MESSAGE_RECEIVED
        );
        while (!client.incoming_queue.empty()) {
            auto packetStack = client.incoming_queue.front();
            client.incoming_queue.pop();

            auto physicalLayerPacket = packetStack.top();
            packetStack.pop();
            auto networkLayerPacket = packetStack.top();
            packetStack.pop();
            auto transportLayerPacket = packetStack.top();
            packetStack.pop();
            auto applicationLayerPacket = packetStack.top();
            packetStack.pop();
            current_relative = stoi(applicationLayerPacket->get_relative_packet_index());
            if (last_relative >= current_relative) {
                firstPacket = true;
            } else {
                firstPacket = false;
            }
            auto receiver_id = applicationLayerPacket->get_receiver();
            auto receiverIndex = findClientIndexWithClientId(clients, receiver_id);
            auto sender_id = clients[findClientIndexWithMAC(clients, physicalLayerPacket->get_sender())].client_id;
            if (clients[receiverIndex].client_id == client.client_id) {
                cout << "Client " << client.client_id << " receiving frame #"
                     << applicationLayerPacket->get_relative_packet_index() << " from client " << sender_id
                     << ", originating from client " << applicationLayerPacket->get_sender();
                physicalLayerPacket->print();
                networkLayerPacket->print();
                transportLayerPacket->print();
                applicationLayerPacket->print();
                applicationLayerPacket->print3();
                physicalLayerPacket->print2();
                printDashes(8);

                if (firstPacket) {
                    if (!log1.sender_id.empty()) {
                        client.log_entries.push_back(log1);
                    }

                    log1.message_content = applicationLayerPacket->get_custom_attribute();
                    log1.number_of_frames = current_relative;
                    log1.number_of_hops = stoi(physicalLayerPacket->get_custom_attribute());
                    log1.sender_id = applicationLayerPacket->get_sender();
                    log1.receiver_id = applicationLayerPacket->get_receiver();
                    log1.success_status = true;
                    log1.activity_type = ActivityType::MESSAGE_RECEIVED;
                } else {
                    log1.message_content += applicationLayerPacket->get_custom_attribute();
                    log1.number_of_frames = current_relative;
                }
            } else {
                auto nextHopClientIndex = findNextHopClientIndex(clients, client.client_id, receiver_id);
                if (nextHopClientIndex < 0) {
                    // drop packages
                    if (firstPacket) {
                        if (!log1.sender_id.empty()) {
                            printDashes(8);
                            client.log_entries.push_back(log1);
                        }

                        log1.message_content = applicationLayerPacket->get_custom_attribute();
                        log1.number_of_frames = current_relative;
                        log1.number_of_hops = stoi(physicalLayerPacket->get_custom_attribute());
                        log1.sender_id = applicationLayerPacket->get_sender();
                        log1.receiver_id = applicationLayerPacket->get_receiver();
                        log1.success_status = false;
                        log1.activity_type = ActivityType::MESSAGE_DROPPED;
                    } else {
                        log1.message_content += applicationLayerPacket->get_custom_attribute();
                        log1.number_of_frames = current_relative;
                    }

                    cout << "Client " << client.client_id << " receiving frame #" << current_relative << " from client "
                         << sender_id << ", but intended for client " << receiver_id << ". Forwarding..." << endl;
                    cout << "Error: Unreachable destination. Packets are dropped after "
                         << physicalLayerPacket->get_custom_attribute() << " hops!" << endl;
                } else {
                    // forward packages
                    if (firstPacket) {
                        if (!log1.sender_id.empty()) {
                            printDashes(8);
                            client.log_entries.push_back(log1);
                        }
                        cout << "Client " << client.client_id << " receiving a message from client " << sender_id
                             << ", but intended for client " << receiver_id << ". Forwarding..." << endl;

                        log1.message_content = applicationLayerPacket->get_custom_attribute();
                        log1.number_of_frames = current_relative;
                        log1.number_of_hops = stoi(physicalLayerPacket->get_custom_attribute());
                        log1.sender_id = applicationLayerPacket->get_sender();
                        log1.receiver_id = applicationLayerPacket->get_receiver();
                        log1.success_status = true;
                        log1.activity_type = ActivityType::MESSAGE_FORWARDED;
                    } else {
                        log1.message_content += applicationLayerPacket->get_custom_attribute();
                        log1.number_of_frames = current_relative;
                    }

                    physicalLayerPacket->set_sender(client.client_mac);
                    physicalLayerPacket->set_receiver(clients[nextHopClientIndex].client_mac);

                    packetStack.push(applicationLayerPacket);
                    packetStack.push(transportLayerPacket);
                    packetStack.push(networkLayerPacket);
                    packetStack.push(physicalLayerPacket);

                    physicalLayerPacket->increment_hop();
                    clients[nextHopClientIndex].incoming_queue.push(packetStack);

                    cout << "Frame #" << applicationLayerPacket->get_relative_packet_index()
                         << " MAC address change: New sender MAC: " << physicalLayerPacket->get_sender()
                         << ", new receiver MAC " << physicalLayerPacket->get_receiver() << endl;

                }
            }

        }
        if (!log1.sender_id.empty()) {
            client.log_entries.push_back(log1);
            printDashes(8);
        }
    }
}

void handlePrintLog(vector<Client> &clients, const string &client_id) {
    auto clientIndex = findClientIndexWithClientId(clients, client_id);
    if (clientIndex < 0) {
        // may there be an error message here
        return;
    }
    clients[clientIndex].printLogEntries();
}

void handleInvalidCommand() {
    cout << "Invalid command." << endl;
}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */

    for (auto &command: commands) {
        printDashes(command.size() + 9);
        cout << "Command: " << command << endl;
        printDashes(command.size() + 9);

        auto splitCommand = splitCustom(command, ' ');
        auto firstWord = splitCommand[0];
        if (firstWord == "MESSAGE") {
            handleMessage(clients, sender_port, receiver_port,
                          splitCommand[1], splitCommand[2], splitCommand[3],
                          message_limit);
        } else if (firstWord == "SHOW_FRAME_INFO") {
            handleShowFrameInfo(clients, splitCommand[1], splitCommand[2], splitCommand[3]);
        } else if (firstWord == "SHOW_Q_INFO") {
            handleShowQueueInfo(clients, splitCommand[1], splitCommand[2]);
        } else if (firstWord == "SEND") {
            handleSend(clients);
        } else if (firstWord == "RECEIVE") {
            handleReceive(clients);
        } else if (firstWord == "PRINT_LOG") {
            handlePrintLog(clients, splitCommand[1]);
        } else {
            handleInvalidCommand();
        }
    }

}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    // Read clients from the given input file and return a vector of Client instances.
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return clients;
    }

    int numClients;
    if (!(file >> numClients)) {
        cerr << "Error reading the clients file." << endl;
        file.close();
        return clients;
    }

    string line;
    getline(file, line);

    for (int i = 0; i < numClients; ++i) {
        if (!getline(file, line)) break;
        string id, ip, mac;

        istringstream lineStream(line);
        lineStream >> id >> ip >> mac;

        Client tempClient(id, ip, mac);
        clients.push_back(tempClient);
    }

    file.close();
    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.

    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    int clientPointer = 0;
    unordered_map<string, string> tempRoutes;

    while (getline(file, line)) {
        if (line[0] == '-') {
            clients[clientPointer++].routing_table = tempRoutes;
            tempRoutes.clear();
            continue;
        }

        auto splitLine = splitCustom(line, ' ');
        tempRoutes[splitLine[0]] = splitLine[1];
    }

    file.close();
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    // Read commands from the given input file and return them as a vector of strings.
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return commands;
    }

    int numClients;
    if (!(file >> numClients)) {
        cerr << "Error reading the commands file." << endl;
        file.close();
        return commands;
    }

    string line;
    getline(file, line);

    for (int i = 0; i < numClients; ++i) {
        if (!getline(file, line)) break;
        commands.push_back(line);
    }

    file.close();
    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.
}
