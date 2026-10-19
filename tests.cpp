#include "rover_controller.h"
#include <cassert>
#include <iostream>

CameraFrame baseFrame() {
    return CameraFrame{1, true, true, 0.0, false, false, {}};
}

int main() {
    RoverController plants(2);

    auto frame = baseFrame();
    assert(plants.decide(frame).action == Action::DriveStraight);

    frame.pathOffset = -0.4;
    assert(plants.decide(frame).action == Action::TurnLeft);

    frame.pathOffset = 0.4;
    assert(plants.decide(frame).action == Action::TurnRight);

    frame = baseFrame();
    frame.tags = {{2, -18.0, 1.2, 0.99}};
    assert(plants.decide(frame).action == Action::TurnLeft);

    frame.tags = {{2, 20.0, 1.2, 0.99}};
    assert(plants.decide(frame).action == Action::TurnRight);

    frame.tags = {{2, 2.0, 1.2, 0.99}};
    assert(plants.decide(frame).action == Action::DriveStraight);

    frame.tags = {{2, 1.0, 0.25, 0.99}};
    assert(plants.decide(frame).action == Action::StopAtTarget);

    frame.tags = {{2, 15.0, 0.25, 0.99}};
    assert(plants.decide(frame).action == Action::HoldPosition);

    frame.tags = {{1, 0.0, 0.25, 0.99}};
    assert(plants.decide(frame).action == Action::DriveStraight);

    frame.tags = {{2, 0.0, 0.25, 0.99}, {900, 0.0, 0.7, 0.90}};
    assert(plants.decide(frame).action == Action::EmergencyStop);

    frame.tags = {{900, 0.0, 0.7, 0.20}};
    assert(plants.decide(frame).action == Action::DriveStraight);

    frame = baseFrame();
    frame.obstacleDetected = true;
    assert(plants.decide(frame).action == Action::EmergencyStop);

    frame = baseFrame();
    frame.bumperPressed = true;
    assert(plants.decide(frame).action == Action::EmergencyStop);

    frame = baseFrame();
    frame.valid = false;
    assert(plants.decide(frame).action == Action::EmergencyStop);

    frame = baseFrame();
    frame.pathVisible = false;
    assert(plants.decide(frame).action == Action::SearchForRoute);

    RoverController dock(500);
    frame = baseFrame();
    frame.tags = {{500, 0.0, 0.2, 0.99}};
    assert(dock.decide(frame).action == Action::StopAtTarget);

    assert(classifyTag(2) == TagType::Plant);
    assert(classifyTag(500) == TagType::Dock);
    assert(classifyTag(900) == TagType::Boundary);
    assert(classifyTag(700) == TagType::Unknown);

    std::cout << "PASS: all controller decision situations\n";
}
