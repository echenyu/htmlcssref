#for having c++11
PATH := /usr/um/gcc-4.7.0/bin:$(PATH)
LD_LIBRARY_PATH := /usr/um/gcc-4.7.0/lib64
LD_RUN_PATH := /usr/um/gcc-4.7.0/lib64

-R -r:
	g++ -std=c++11 -O3 stockMarket.cpp -o market

debug:
	g++ -std=c++11 -g stockMarket.cpp -o market

clean:
	rm -f $(SOURCES:%.cpp=%.o) stockMarket market submit.tar.gz