#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Leaderboard.h"

void Leaderboard::insert_new_entry(LeaderboardEntry *new_entry) {
    // Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
    // is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
    // top 10 all-time high-scores should be kept in descending order by the score).

    if (this->head_leaderboard_entry == nullptr) {
        this->head_leaderboard_entry = new_entry;
        return;
    }

    if (new_entry->score > this->head_leaderboard_entry->score) {
        new_entry->next_leaderboard_entry = this->head_leaderboard_entry;
        this->head_leaderboard_entry = new_entry;
    } else {
        auto prevPointer = this->head_leaderboard_entry;
        auto currentPointer = this->head_leaderboard_entry->next_leaderboard_entry;
        while (currentPointer != nullptr
               && new_entry->score <= currentPointer->score) {
            prevPointer = currentPointer;
            currentPointer = currentPointer->next_leaderboard_entry;
        }
        if (currentPointer == nullptr) {
            prevPointer->next_leaderboard_entry = new_entry;
        } else {
            prevPointer->next_leaderboard_entry = new_entry;
            new_entry->next_leaderboard_entry = currentPointer;
        }
    }

    int count = 0;
    auto currentPointer = this->head_leaderboard_entry;
    while (currentPointer != nullptr) {
        count++;
        currentPointer = currentPointer->next_leaderboard_entry;
    }
    currentPointer = this->head_leaderboard_entry;
    if (count > 10) {
        for (int i = 0; i < 9; ++i) {
            currentPointer = currentPointer->next_leaderboard_entry;
        }
        auto deleteAfterIncluding = currentPointer->next_leaderboard_entry;
        currentPointer->next_leaderboard_entry = nullptr;
        std::vector<LeaderboardEntry *> deleteAll;
        while (deleteAfterIncluding != nullptr) {
            deleteAll.push_back(deleteAfterIncluding);
            deleteAfterIncluding = deleteAfterIncluding->next_leaderboard_entry;
        }
        for (auto pointer: deleteAll) {
            delete pointer;
        }
    }
}

void Leaderboard::write_to_file(const string &filename) {
    // Write the latest leaderboard status to the given file in the format specified in the PA instructions

    ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening leaderboard file named: " << filename << std::endl;
        exit(4);
    }

    LeaderboardEntry *current = this->head_leaderboard_entry;
    while (current != nullptr) {
        file << current->score << " " << current->last_played << " " << current->player_name << std::endl;
        current = current->next_leaderboard_entry;
    }

    file.close();
}

void Leaderboard::read_from_file(const string &filename) {
    // Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
    // variable will point to the highest all-times score, and all other scores will be reachable from it
    // via the "next_leaderboard_entry" member variable pointer.

    ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening leaderboard file named: " << filename << std::endl;
        exit(3);
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        unsigned long score;
        time_t last_played;
        string player_name;

        if (!(iss >> score >> last_played >> player_name)) {
            std::cerr << "Error parsing line: " << line << std::endl;
            continue;
        }

        auto newEntry = new LeaderboardEntry(score, last_played, player_name);

        insert_new_entry(newEntry);
    }

    file.close();
}


void Leaderboard::print_leaderboard() {
    // Print the current leaderboard status to the standard output in the format specified in the PA instructions

    LeaderboardEntry *current = this->head_leaderboard_entry;
    int order = 1;

    std::cout << "Leaderboard" << std::endl;
    std::cout << "-----------" << std::endl;

    while (current != nullptr) {
        char timestamp[20];
        std::strftime(timestamp,
                      sizeof(timestamp),
                      "%H:%M:%S/%d.%m.%Y",
                      std::localtime(&current->last_played));

        std::cout << order << ". "
                  << current->player_name << " "
                  << current->score << " "
                  << timestamp << std::endl;

        current = current->next_leaderboard_entry;
        order++;
    }
}

Leaderboard::~Leaderboard() {
    // Free dynamically allocated memory used for storing leaderboard entries

    auto deleteAfterIncluding = this->head_leaderboard_entry;
    std::vector<LeaderboardEntry *> deleteAll;
    while (deleteAfterIncluding != nullptr) {
        deleteAll.push_back(deleteAfterIncluding);
        deleteAfterIncluding = deleteAfterIncluding->next_leaderboard_entry;
    }
    for (auto pointer: deleteAll) {
        delete pointer;
    }
}
