# enables c++11 on CAEN
PATH := /usr/um/gcc-4.7.0/bin:$(PATH)
LD_LIBRARY_PATH := /usr/um/gcc-4.7.0/lib64
LD_RUN_PATH := /usr/um/gcc-4.7.0/lib64

# name of the tar ball created for submission
SUBMIT_FILE = submit.tar.gz

-R -r:
	g++  -std=c++11 p2.cpp -O3 -o MineEscape

clean:
	rm -f $(SOURCES:%.cpp=%.o) p2 submit.tar.gz MineEscape poormantest

$(SUBMIT_FILE): $(MY_FILES)
	dos2unix $(MY_FILES)
	tar -vczf $(SUBMIT_FILE) $(MY_FILES)

# shortcut for make submit.tar.gz (make submit)
submit: submit.tar.gz