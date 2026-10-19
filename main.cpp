#include "rover_controller.h"
#include "simulation_io.h"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    const std::string dataPath = argc > 1 ? argv[1] : "simulated_camera_data.csv";
    const int targetTagId = argc > 2 ? std::stoi(argv[2]) : 2;

    try {
        RoverController controller(targetTagId);
        const auto frames = loadCameraFramesFile(dataPath);
        for (const auto& frame : frames) {
            const Decision decision = controller.decide(frame);
            std::cout << "Frame " << frame.frameNumber << ": "
                      << actionName(decision.action) << " | "
                      << decision.reason << '\n';
        }
    } catch (const std::exception& error) {
        std::cerr << "Simulation error: " << error.what() << '\n';
        return 1;
    }
}
