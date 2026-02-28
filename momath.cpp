#include <iostream>
#include <iomanip>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <random>

static const int SIMULATION_STEPS = 10;


enum class State : char {
    Empty,
    Full
};

class SimulationGrid {
public:
    static const int GRID_SIZE = 500;
    static const int VIEW_SIZE = 40;

    std::vector<State> grid;

    SimulationGrid() : grid(GRID_SIZE * GRID_SIZE, State::Empty) {}

    State get(int x, int y) const {
        return grid[y * GRID_SIZE + x];
    }

    void set(int x, int y, State s) {
        grid[y * GRID_SIZE + x] = s;
    }

    int countTotalBacteria() const {
        int count = 0;
        for (auto s : grid) if (s == State::Full) count++;
        return count;
    }
};

void displayViewport(const SimulationGrid &sg) {
    system("clear");
    
    for (int y = SimulationGrid::VIEW_SIZE - 1; y >= 0; y--) {
        std::cout << std::setw(2) << y << "| ";
        for (int x = 0; x < SimulationGrid::VIEW_SIZE; x++) {
            std::cout << (sg.get(x, y) == State::Empty ? ". " : "O ");
        }
        std::cout << "\n";
    }
    std::cout << "   " << std::string(SimulationGrid::VIEW_SIZE * 2, '-') << "\n";
    for (int x = 0; x <= SimulationGrid::VIEW_SIZE; x += 2) std::cout << std::setw(4) << x;
    std::cout << "\n";
}

bool updateMovement(SimulationGrid &sg) {
    bool changed = false;
    SimulationGrid nextStep = sg;

    std::vector<std::pair<int, int>> parents;
    for (int y = 0; y < SimulationGrid::GRID_SIZE; y++) {
        for (int x = 0; x < SimulationGrid::GRID_SIZE; x++) {
            if (sg.get(x, y) == State::Full) parents.push_back({x, y});
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(parents.begin(), parents.end(), g);

    for (auto const& [x, y] : parents) {
        int ex = x + 1, ey = y;    
        int nx = x, ny = y + 1;
        if (ex < SimulationGrid::GRID_SIZE && ny < SimulationGrid::GRID_SIZE) {
            if (sg.get(ex, ey) == State::Empty && sg.get(nx, ny) == State::Empty &&
                nextStep.get(ex, ey) == State::Empty && nextStep.get(nx, ny) == State::Empty) {
                
                nextStep.set(x, y, State::Empty);
                nextStep.set(ex, ey, State::Full);
                nextStep.set(nx, ny, State::Full);
                changed = true;
            }
        }
    }
    sg = nextStep;
    return changed;
}

int main() {
    SimulationGrid sg;
    sg.set(0, 0, State::Full);

    std::ofstream outFile("simulation_results.txt");
    if (!outFile.is_open()) return 1;
    outFile << "Step,BacteriaCount\n";

    int steps = 0;
    while (true) {
        int count = sg.countTotalBacteria();
        outFile << steps << "," << count << "\n";
        
        displayViewport(sg);
        std::cout << "Step: " << steps << " | Total Bacteria: " << count << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (!updateMovement(sg)) break;
        steps++;
        
        if (steps > SIMULATION_STEPS) break; 
    }

    outFile.close();
    return 0;
}