#include "rover_controller.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr double kStopDistanceM = 0.35;
constexpr double kAlignmentToleranceDeg = 5.0;
constexpr double kPathTolerance = 0.10;
constexpr double kTargetConfidence = 0.75;
constexpr double kBoundaryConfidence = 0.60;

bool usable(const TagDetection& tag, double minimumConfidence) {
    return tag.confidence >= minimumConfidence &&
           tag.distanceM > 0.0 &&
           tag.distanceM <= 20.0 &&
           std::abs(tag.angleDeg) <= 180.0;
}
}

TagType classifyTag(int id) {
    if (id >= 1 && id <= 99) return TagType::Plant;
    if (id == 500) return TagType::Dock;
    if (id >= 900 && id <= 999) return TagType::Boundary;
    return TagType::Unknown;
}

std::string actionName(Action action) {
    switch (action) {
        case Action::TurnLeft: return "TURN_LEFT";
        case Action::TurnRight: return "TURN_RIGHT";
        case Action::DriveStraight: return "DRIVE_STRAIGHT";
        case Action::StopAtTarget: return "STOP_AT_TARGET";
        case Action::HoldPosition: return "HOLD_POSITION";
        case Action::EmergencyStop: return "EMERGENCY_STOP";
        case Action::SearchForRoute: return "SEARCH_FOR_ROUTE";
    }
    return "UNKNOWN_ACTION";
}

RoverController::RoverController(int targetTagId)
    : targetTagId_(targetTagId) {}

Decision RoverController::decide(const CameraFrame& frame) const {
    if (!frame.valid) {
        return {Action::EmergencyStop, "camera frame invalid or stale"};
    }
    if (frame.bumperPressed) {
        return {Action::EmergencyStop, "bumper pressed"};
    }
    if (frame.obstacleDetected) {
        return {Action::EmergencyStop, "camera reports an obstacle"};
    }

    for (const auto& tag : frame.tags) {
        if (classifyTag(tag.id) == TagType::Boundary && usable(tag, kBoundaryConfidence)) {
            return {Action::EmergencyStop, "boundary AprilTag visible"};
        }
    }

    const TagDetection* target = nullptr;
    for (const auto& tag : frame.tags) {
        if (tag.id == targetTagId_ && usable(tag, kTargetConfidence) &&
            (target == nullptr || tag.distanceM < target->distanceM)) {
            target = &tag;
        }
    }

    if (target != nullptr) {
        const bool close = target->distanceM <= kStopDistanceM;
        const bool aligned = std::abs(target->angleDeg) <= kAlignmentToleranceDeg;
        if (close && aligned) {
            return {Action::StopAtTarget, "correct target is centered and within stopping distance"};
        }
        if (close && !aligned) {
            return {Action::HoldPosition, "target is too close to turn safely"};
        }
        if (target->angleDeg < -kAlignmentToleranceDeg) {
            return {Action::TurnLeft, "target AprilTag is left of center"};
        }
        if (target->angleDeg > kAlignmentToleranceDeg) {
            return {Action::TurnRight, "target AprilTag is right of center"};
        }
        return {Action::DriveStraight, "target AprilTag is centered but still far away"};
    }

    if (!frame.pathVisible) {
        return {Action::SearchForRoute, "neither target nor route is visible"};
    }
    if (frame.pathOffset < -kPathTolerance) {
        return {Action::TurnLeft, "route center is left of camera center"};
    }
    if (frame.pathOffset > kPathTolerance) {
        return {Action::TurnRight, "route center is right of camera center"};
    }
    return {Action::DriveStraight, "route is centered"};
}
