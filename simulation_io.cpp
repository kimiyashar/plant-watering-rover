#include "simulation_io.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {
std::vector<std::string> splitCsv(const std::string& line) {
    std::vector<std::string> fields;
    std::istringstream stream(line);
    std::string field;
    while (std::getline(stream, field, ',')) fields.push_back(field);
    return fields;
}

bool parseBool(const std::string& value) {
    if (value == "true" || value == "1") return true;
    if (value == "false" || value == "0") return false;
    throw std::runtime_error("invalid boolean in simulation CSV: " + value);
}
}

std::vector<CameraFrame> loadCameraFrames(std::istream& input) {
    std::vector<CameraFrame> frames;
    std::string line;
    bool firstDataLine = true;

    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (firstDataLine && line.rfind("frame,", 0) == 0) {
            firstDataLine = false;
            continue;
        }
        firstDataLine = false;

        const auto fields = splitCsv(line);
        if (fields.size() != 10) {
            throw std::runtime_error("expected 10 CSV fields: " + line);
        }

        const int frameNumber = std::stoi(fields[0]);
        if (frames.empty() || frames.back().frameNumber != frameNumber) {
            frames.push_back(CameraFrame{
                frameNumber,
                parseBool(fields[1]),
                parseBool(fields[2]),
                std::stod(fields[3]),
                parseBool(fields[4]),
                parseBool(fields[5]),
                {}
            });
        }

        const int tagId = std::stoi(fields[6]);
        if (tagId >= 0) {
            frames.back().tags.push_back(TagDetection{
                tagId,
                std::stod(fields[7]),
                std::stod(fields[8]),
                std::stod(fields[9])
            });
        }
    }
    return frames;
}

std::vector<CameraFrame> loadCameraFramesFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("could not open simulation file: " + path);
    return loadCameraFrames(file);
}
