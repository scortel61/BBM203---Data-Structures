#include <sstream>
#include <fstream>
#include <iostream>
#include "BlockFall.h"


Block *newBlock() {
    auto *created = new Block;
    created->left_rotation = nullptr;
    created->right_rotation = nullptr;
    created->next_block = nullptr;
    return created;
}

// rotation creation
std::vector<std::vector<bool>> grid_rotate_90_degree_clockwise(const std::vector<std::vector<bool>> &input) {
    if (input.empty() || input[0].empty()) {
        return {};
    }

    std::vector<std::vector<bool>> rotated;
    for (int j = 0; j < input.at(0).size(); ++j) {
        vector<bool> temp;
        for (int i = input.size() - 1; i >= 0; --i) {
            temp.push_back(input[i][j]);
        }
        rotated.push_back(temp);
    }

    return rotated;
}


Block *createBlockAndRotations(const vector<vector<bool>> &original_shape) {
    auto rotated_90 = grid_rotate_90_degree_clockwise(original_shape);
    auto rotated_180 = grid_rotate_90_degree_clockwise(rotated_90);
    auto rotated_270 = grid_rotate_90_degree_clockwise(rotated_180);

    auto original_shape_block = newBlock();
    original_shape_block->shape = original_shape;
    auto rotated_90_block = newBlock();
    rotated_90_block->shape = rotated_90;
    auto rotated_180_block = newBlock();
    rotated_180_block->shape = rotated_180;
    auto rotated_270_block = newBlock();
    rotated_270_block->shape = rotated_270;

    if (*original_shape_block == *rotated_90_block) {
        if (*rotated_90_block == *rotated_180_block) {
            original_shape_block->right_rotation = original_shape_block;
            original_shape_block->left_rotation = original_shape_block;

            delete rotated_90_block;
            delete rotated_180_block;
            delete rotated_270_block;
        } else {
            original_shape_block->right_rotation = rotated_90_block;
            original_shape_block->left_rotation = rotated_90_block;

            rotated_90_block->right_rotation = original_shape_block;
            rotated_90_block->left_rotation = original_shape_block;

            delete rotated_180_block;
            delete rotated_270_block;
        }
        return original_shape_block;
    }

    original_shape_block->right_rotation = rotated_90_block;
    original_shape_block->left_rotation = rotated_270_block;

    rotated_90_block->right_rotation = rotated_180_block;
    rotated_90_block->left_rotation = original_shape_block;

    rotated_180_block->right_rotation = rotated_270_block;
    rotated_180_block->left_rotation = rotated_90_block;

    rotated_270_block->right_rotation = original_shape_block;
    rotated_270_block->left_rotation = rotated_180_block;

    return original_shape_block;
}

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on,
                     const string &leaderboard_file_name, const string &player_name) : gravity_mode_on(
        gravity_mode_on), leaderboard_file_name(leaderboard_file_name), player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}

void BlockFall::read_blocks(const string &input_file) {
    // Read the blocks from the input file and initialize "initial_block" and "active_rotation" member variables
    // For every block, generate its rotations and properly implement the multilevel linked list structure
    // that represents the game blocks, as explained in the PA instructions.
    // Initialize the "power_up" member variable as the last block from the input file (do not add it to the linked list!)

    std::ifstream inputFile(input_file);

    if (!inputFile.is_open()) {
        std::cerr << "Could not open the blocks file named: " << input_file << std::endl;
        exit(2);
    }

    std::string line;
    std::vector<bool> currentRow;
    std::vector<std::vector<bool>> currentShape;
    std::vector<std::vector<std::vector<bool>>> shapes;

    while (std::getline(inputFile, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        char c;

        if (ss >> c) {
            currentRow.clear();
            if (c != '[') {
                currentRow.push_back(c == '1');
            }

            while (ss >> c && c != ']') {
                currentRow.push_back(c == '1');
            }

            currentShape.push_back(currentRow);

            if (c == ']') {
                shapes.push_back(currentShape);
                currentShape.clear();
            }
        }
    }

    this->power_up = shapes.at(shapes.size() - 1);
    shapes.pop_back();

    if (!shapes.empty()) {
        auto prevBlock = createBlockAndRotations(shapes.at(0));
        this->initial_block = prevBlock;
        this->active_rotation = prevBlock;
        for (int i = 1; i < shapes.size(); ++i) {
            auto newlyCreatedBlock = createBlockAndRotations(shapes.at(i));
            prevBlock->next_block = newlyCreatedBlock;
            prevBlock = newlyCreatedBlock;
        }
        auto currentPointer = this->initial_block;
        while (currentPointer != nullptr) {
            auto next_block = currentPointer->next_block;
            currentPointer->right_rotation->next_block = next_block;
            currentPointer->left_rotation->next_block = next_block;
            currentPointer->right_rotation->right_rotation->next_block = next_block;
            currentPointer = next_block;
        }
    }
}


void BlockFall::initialize_grid(const string &input_file) {
    // Initialize "rows" and "cols" member variables
    // Initialize "grid" member variable using the command-line argument 1 in main

    std::ifstream file(input_file);

    if (!file.is_open()) {
        std::cerr << "Could not open the grid file named: " << input_file << std::endl;
        exit(1);
    }

    std::string line;
    rows = 0;
    while (std::getline(file, line)) {
        ++rows;
        std::istringstream iss(line);
        std::vector<int> row;

        int value;
        while (iss >> value) {
            row.push_back(value);
        }

        grid.push_back(row);
    }

    if (this->grid.size() > 0) {
        this->cols = this->grid.at(0).size();
    }

    file.close();
}


BlockFall::~BlockFall() {
    // Free dynamically allocated memory used for storing game blocks
    delete this->initial_block;
}
