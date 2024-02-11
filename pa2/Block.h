#ifndef PA2_BLOCK_H
#define PA2_BLOCK_H

#include <vector>

using namespace std;

class Block {
public:

    vector<vector<bool>> shape; // Two-dimensional vector corresponding to the block's shape
    Block *right_rotation = nullptr; // Pointer to the block's clockwise neighbor block (its right rotation)
    Block *left_rotation = nullptr; // Pointer to the block's counter-clockwise neighbor block (its left rotation)
    Block *next_block = nullptr; // Pointer to the next block to appear in the game

    bool operator==(const Block &other) const {
        if (this->shape.size() != other.shape.size()) {
            return false;
        }
        for (int i = 0; i < this->shape.size(); ++i) {
            if (this->shape.at(i).size() != other.shape.at(i).size()) {
                return false;
            }
            for (int j = 0; j < this->shape.at(i).size(); ++j) {
                if (this->shape.at(i).at(j) != other.shape.at(i).at(j)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Block &other) const {
        return !(this->operator==(other));
    }

    ~Block() {
        if (this->right_rotation == nullptr) {
            return;
        }
        if (this->right_rotation == this) {
            return;
        }
        if (this->right_rotation->right_rotation == this) {
            this->right_rotation->right_rotation = nullptr;
            delete this->right_rotation;
            this->right_rotation = nullptr;
            return;
        }
        auto toBeDeleted = this->right_rotation->right_rotation;
        toBeDeleted->right_rotation = nullptr;
        delete toBeDeleted;
        toBeDeleted = this->right_rotation;
        toBeDeleted->right_rotation = nullptr;
        delete toBeDeleted;
        toBeDeleted = this->left_rotation;
        toBeDeleted->right_rotation = nullptr;
        delete toBeDeleted;

        delete next_block;
    }
};


#endif //PA2_BLOCK_H
