#for having c++11
PATH := /usr/um/gcc-4.7.0/bin:$(PATH)
LD_LIBRARY_PATH := /usr/um/gcc-4.7.0/lib64
LD_RUN_PATH := /usr/um/gcc-4.7.0/lib64

-R -r:
	g++ -std=c++11 -O3 droneRoute.cpp -o DroneRouting

debug:
	g++ -std=c++11 -g droneRoute.cpp -Wall -Werror -Wextra -o DroneRouting

clean:
	rm -f $(SOURCES:%.cpp=%.o) DroneRouting market submit.tar.gz