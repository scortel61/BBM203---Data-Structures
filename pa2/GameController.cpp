#include <iostream>
#include <fstream>
#include <map>
#include "GameController.h"

static bool GAME_END_FLAG = false;
static bool GAME_END_VALUE = true;

void handlePrintBeforeCleaning(BlockFall &game) {
    std::cout << "Before clearing:" << std::endl;
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            std::cout << (game.grid[i][j] ? occupiedCellChar : unoccupiedCellChar);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool validCheck(BlockFall &game, Block *block, int startRow, int startColumn) {
    if (startColumn < 0 || startRow < 0) {
        return false;
    }
    int blockRowCount = block->shape.size();
    if (startRow + blockRowCount > game.rows) {
        return false;
    }

    int blockColumnCount = block->shape.at(0).size();
    if (startColumn + blockColumnCount > game.cols) {
        return false;
    }
    for (int i = startRow; i < startRow + blockRowCount; ++i) {
        int relativeI = i - startRow;
        for (int j = startColumn; j < startColumn + blockColumnCount; ++j) {
            int relativeJ = j - startColumn;
            bool gridElement = game.grid[i][j];
            bool blockElement = block->shape[relativeI][relativeJ];
            if (gridElement && blockElement) {
                return false;
            }
        }
    }
    return true;
}

void addBlockToGrid(vector<vector<int>> &grid, Block *block, int startRow, int startColumn) {
    int blockRowCount = block->shape.size();
    int blockColumnCount = block->shape.at(0).size();
    for (int i = startRow; i < startRow + blockRowCount; ++i) {
        int relativeI = i - startRow;
        for (int j = startColumn; j < startColumn + blockColumnCount; ++j) {
            int relativeJ = j - startColumn;
            if (block->shape[relativeI][relativeJ]) {
                grid[i][j] = block->shape[relativeI][relativeJ];
            }
        }
    }
}

void addBlockToGrid(BlockFall &game, Block *block, int startRow, int startColumn) {
    addBlockToGrid(game.grid, block, startRow, startColumn);
}

std::map<int, int> columnCellCounts(Block *block) {
    std::map<int, int> result;
    auto shape = block->shape;
    for (int i = 0; i < shape.at(0).size(); ++i) {
        result[i] = 0;
    }
    for (int i = 0; i < shape.size(); ++i) {
        for (int j = 0; j < shape.at(0).size(); ++j) {
            if (shape[i][j]) {
                result[j] += 1;
            }
        }
    }
    return result;
}

void addBlockToGridWithGravity(vector<vector<int>> &grid, Block *block, int startColumn) {
    auto columnCells = columnCellCounts(block);
    for (int j = 0; j < grid.at(0).size(); ++j) {
        int realJ = j + startColumn;
        for (int i = grid.size() - 1; i >= 0; --i) {
            if (columnCells[j] > 0) {
                if (!grid[i][realJ]) {
                    grid[i][realJ] = 1;
                    columnCells[j] -= 1;
                }
            }
        }
    }
}

void addBlockToGridWithGravity(BlockFall &game, Block *block, int startColumn) {
    addBlockToGridWithGravity(game.grid, block, startColumn);
}

void handlePrintLeaderBoard(BlockFall &game) {
    game.leaderboard.print_leaderboard();
}

void handlePrintGrid(BlockFall &game, int &currentRow, int &currentColumn, bool applyShape = true) {
    auto tempGrid = game.grid;
    if (applyShape) {
        addBlockToGrid(tempGrid, game.active_rotation, currentRow, currentColumn);
    }
    std::cout << "Score: " << game.current_score << std::endl;
    unsigned long highestScore = max(game.current_score, game.leaderboard.head_leaderboard_entry->score);
    std::cout << "High Score: " << highestScore << std::endl;
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            std::cout << (tempGrid[i][j] ? occupiedCellChar : unoccupiedCellChar);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void handleScoreSave(BlockFall &game) {
    auto *newScore = new LeaderboardEntry(game.current_score, time(nullptr), game.player_name);
    game.leaderboard.insert_new_entry(newScore);
}

void handleWin(BlockFall &game) {
    handleScoreSave(game);

    std::cout << "YOU WIN!" << std::endl;
    std::cout << "No more blocks." << std::endl;
    std::cout << "Final grid and score:" << std::endl;
    std::cout << std::endl;

    int temp = 0, temp1 = 0;
    handlePrintGrid(game, temp, temp1, false);
    handlePrintLeaderBoard(game);
    GAME_END_FLAG = true;
}

void handleGameFinish(BlockFall &game) {
    handleScoreSave(game);

    std::cout << "GAME FINISHED!" << std::endl;
    std::cout << "No more commands." << std::endl;
    std::cout << "Final grid and score:" << std::endl;

    int temp = 0, temp1 = 0;
    handlePrintGrid(game, temp, temp1, false);
    handlePrintLeaderBoard(game);
    GAME_END_FLAG = true;
}

void handleGameOver(BlockFall &game) {
    handleScoreSave(game);

    std::cout << "GAME OVER!" << std::endl;
    std::cout << "Next block that couldn't fit:" << std::endl;
    for (int i = 0; i < game.active_rotation->shape.size(); ++i) {
        for (int j = 0; j < game.active_rotation->shape.at(0).size(); ++j) {
            std::cout << (game.active_rotation->shape[i][j] ? occupiedCellChar : unoccupiedCellChar);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Final grid and score:" << std::endl;
    std::cout << std::endl;

    int temp = 0, temp1 = 0;
    handlePrintGrid(game, temp, temp1, false);
    handlePrintLeaderBoard(game);

    GAME_END_FLAG = true;
    GAME_END_VALUE = false;
}

void handleRotateRight(BlockFall &game, int &currentRow, int &currentColumn) {
    if (validCheck(game, game.active_rotation->right_rotation, currentRow, currentColumn)) {
        game.active_rotation = game.active_rotation->right_rotation;
    }
}

void handleRotateLeft(BlockFall &game, int &currentRow, int &currentColumn) {
    if (validCheck(game, game.active_rotation->left_rotation, currentRow, currentColumn)) {
        game.active_rotation = game.active_rotation->left_rotation;
    }
}

void handleMoveRight(BlockFall &game, int &currentRow, int &currentColumn) {
    if (validCheck(game, game.active_rotation, currentRow, currentColumn + 1)) {
        currentColumn += 1;
    }
}

void handleMoveLeft(BlockFall &game, int &currentRow, int &currentColumn) {
    if (validCheck(game, game.active_rotation, currentRow, currentColumn - 1)) {
        currentColumn -= 1;
    }
}

bool checkIfSame(vector<vector<bool>> &shape, BlockFall &game, int startRow, int startColumn) {
    if (startRow + shape.size() > game.rows) {
        return false;
    }
    if (startColumn + shape.at(0).size() > game.cols) {
        return false;
    }
    for (int i = 0; i < shape.size(); ++i) {
        int relativeI = i + startRow;
        for (int j = 0; j < shape.at(0).size(); ++j) {
            int relativeJ = j + startColumn;
            bool gridVal = game.grid[relativeI][relativeJ];
            bool shapeVal = shape[i][j];
            if (gridVal != shapeVal) return false;
        }
    }

    return true;
}

void handlePowerUp(BlockFall &game) {
    handlePrintBeforeCleaning(game);
    int pointsFromDestroyedCells = 0;
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            if (game.grid[i][j]) {
                pointsFromDestroyedCells++;
            }
            game.grid[i][j] = 0;
        }
    }
    game.current_score += 1000;
    game.current_score += pointsFromDestroyedCells;
}

void handlePowerUpCheck(BlockFall &game) {
    auto powerUp = game.power_up;
    for (int i = 0; i < game.rows; ++i) {
        for (int j = 0; j < game.cols; ++j) {
            if (checkIfSame(powerUp, game, i, j)) {
                handlePowerUp(game);
                return;
            }
        }
    }
}

void handleRowClear(BlockFall &game, int rowIndex) {
    for (int i = rowIndex; i >= 0; --i) {
        for (int j = 0; j < game.cols; ++j) {
            if (i == 0) {
                game.grid[i][j] = 0;
            } else {
                game.grid[i][j] = game.grid[i - 1][j];
            }
        }
    }
    game.current_score += game.cols;
}

void handleFullyFilledRowCheck(BlockFall &game) {
    vector<int> toBeClearedRowIndexes;
    for (int i = 0; i < game.rows; ++i) {
        bool foundGap = false;
        for (int j = 0; j < game.cols; ++j) {
            if (foundGap) continue;

            if (!game.grid[i][j]) {
                foundGap = true;
            }
        }
        if (!foundGap) toBeClearedRowIndexes.push_back(i);
    }

    if (!toBeClearedRowIndexes.empty()) {
        handlePrintBeforeCleaning(game);
    }

    for (auto index: toBeClearedRowIndexes) {
        handleRowClear(game, index);
    }
}

int cellCount(Block *block) {
    int count = 0;
    auto shape = block->shape;
    for (const auto &row: shape) {
        for (const auto &cell: row) {
            if (cell) {
                count++;
            }
        }
    }
    return count;
}

void handleDrop(BlockFall &game, int &currentRow, int &currentColumn) {
    int newRow = currentRow;
    int newColumn = currentColumn;
    while (validCheck(game, game.active_rotation, newRow + 1, newColumn)) {
        newRow++;
    }
    game.current_score += cellCount(game.active_rotation) * (newRow - currentRow);
    if (!game.gravity_mode_on) {
        addBlockToGrid(game, game.active_rotation, newRow, newColumn);
    } else {
        addBlockToGridWithGravity(game, game.active_rotation, newColumn);
    }

    handlePowerUpCheck(game);
    handleFullyFilledRowCheck(game);

    auto usedBlock = game.initial_block;
    game.initial_block = usedBlock->next_block;
    game.active_rotation = game.initial_block;
    usedBlock->next_block = nullptr;
    delete usedBlock;

    currentRow = 0;
    currentColumn = 0;

    if (game.initial_block == nullptr) {
        handleWin(game);
    } else {
        if (!validCheck(game, game.active_rotation, currentRow, currentColumn)) {
            handleGameOver(game);
        }
    }
}

void handleGravityOn(BlockFall &game) {
    for (int col = 0; col < game.cols; ++col) {
        int currentRow = game.rows - 1;

        for (int row = game.rows - 1; row >= 0; --row) {
            if (game.grid[row][col] == 1) {
                game.grid[currentRow][col] = 1;
                if (currentRow != row) {
                    game.grid[row][col] = 0;
                }
                --currentRow;
            }
        }
    }

    handleFullyFilledRowCheck(game);
}

void handleGravitySwitch(BlockFall &game) {
    game.gravity_mode_on = !game.gravity_mode_on;
    if (game.gravity_mode_on) {
        handleGravityOn(game);
    }
}

bool GameController::play(BlockFall &game, const string &commands_file) {
    // Implement the gameplay here while reading the commands from the input file given as the 3rd tempCommand-line
    // argument. The return value represents if the gameplay was successful or not: false if game over,
    // true otherwise.

    std::ifstream inputFile(commands_file);

    if (!inputFile.is_open()) {
        std::cerr << "Could not open the blocks file named: " << commands_file << std::endl;
        exit(5);
    }

    std::vector<std::string> commands;

    std::string tempCommand;
    while (inputFile >> tempCommand) {
        commands.push_back(tempCommand);
    }

    inputFile.close();

    int currentRow = 0;
    int currentColumn = 0;

    if (!validCheck(game, game.active_rotation, currentRow, currentColumn)) {
        handleGameOver(game);
    }

    if (!GAME_END_FLAG) {
        for (const auto &command: commands) {
            if (command == "PRINT_GRID") {
                handlePrintGrid(game, currentRow, currentColumn);
            } else if (command == "ROTATE_RIGHT") {
                handleRotateRight(game, currentRow, currentColumn);
            } else if (command == "ROTATE_LEFT") {
                handleRotateLeft(game, currentRow, currentColumn);
            } else if (command == "MOVE_RIGHT") {
                handleMoveRight(game, currentRow, currentColumn);
            } else if (command == "MOVE_LEFT") {
                handleMoveLeft(game, currentRow, currentColumn);
            } else if (command == "DROP") {
                handleDrop(game, currentRow, currentColumn);
            } else if (command == "GRAVITY_SWITCH") {
                handleGravitySwitch(game);
            } else {
                std::cerr << "Unknown command: " << command << std::endl;
            }
            if (GAME_END_FLAG) {
                break;
            }
        }
    }

    if (!GAME_END_FLAG) {
        handleGameFinish(game);
    }

    game.leaderboard.write_to_file(game.leaderboard_file_name);

    return GAME_END_VALUE;
}

GameController::GameController() = default;



