CXX := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic

.PHONY: all test clean

all: rover_sim

rover_sim: main.cpp rover_controller.cpp rover_controller.h simulation_io.cpp simulation_io.h
	$(CXX) $(CXXFLAGS) main.cpp rover_controller.cpp simulation_io.cpp -o rover_sim

controller_tests: tests.cpp rover_controller.cpp rover_controller.h
	$(CXX) $(CXXFLAGS) tests.cpp rover_controller.cpp -o controller_tests

simulation_io_tests: simulation_io_tests.cpp simulation_io.cpp simulation_io.h rover_controller.h
	$(CXX) $(CXXFLAGS) simulation_io_tests.cpp simulation_io.cpp -o simulation_io_tests

test: controller_tests simulation_io_tests
	./controller_tests
	./simulation_io_tests

clean:
	rm -f rover_sim controller_tests simulation_io_tests tests
