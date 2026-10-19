#include "simulation_io.h"
#include <cassert>
#include <iostream>
#include <sstream>

int main() {
    std::istringstream input(
        "frame,valid,path_visible,path_offset,obstacle,bumper,tag_id,tag_angle_deg,tag_distance_m,tag_confidence\n"
        "1,true,true,0.0,false,false,-1,0,0,0\n"
        "2,true,true,0.0,false,false,2,-10,1.2,0.99\n"
        "2,true,true,0.0,false,false,900,5,0.8,0.95\n"
    );
    auto frames = loadCameraFrames(input);
    assert(frames.size() == 2);
    assert(frames[0].tags.empty());
    assert(frames[1].tags.size() == 2);
    assert(frames[1].tags[0].id == 2);
    assert(frames[1].tags[1].id == 900);
    std::cout << "PASS: CSV camera simulation loader\n";
}
