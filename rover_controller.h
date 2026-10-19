#pragma once

#include <string>
#include <vector>

enum class TagType { Plant, Dock, Boundary, Unknown };
enum class Action {
    TurnLeft,
    TurnRight,
    DriveStraight,
    StopAtTarget,
    HoldPosition,
    EmergencyStop,
    SearchForRoute
};

struct TagDetection {
    int id;
    double angleDeg;
    double distanceM;
    double confidence;
};

struct CameraFrame {
    int frameNumber;
    bool valid;
    bool pathVisible;
    double pathOffset;
    bool obstacleDetected;
    bool bumperPressed;
    std::vector<TagDetection> tags;
};

struct Decision {
    Action action;
    std::string reason;
};

TagType classifyTag(int id);
std::string actionName(Action action);

class RoverController {
public:
    explicit RoverController(int targetTagId);
    Decision decide(const CameraFrame& frame) const;
private:
    int targetTagId_;
};
