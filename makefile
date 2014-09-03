EXECUTABLE := text_to_cpp_string
CXX	:= g++
CXXFLAGS := -g -std=c++0x
INCLUDES := -I. -I./src

ifeq ($(OS),Windows_NT)
    #CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        #CCFLAGS += -D AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        #CCFLAGS += -D IA32
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        #CCFLAGS += -D LINUX
        INCLUDES += -I../boost_uusi/usr_local_include/
		LIBS     := -L../boost_linux32/usr_local_lib/ -L/usr/lib/ -lboost_chrono -lboost_filesystem -lboost_system
    endif
    ifeq ($(UNAME_S),Darwin)
        #CCFLAGS += -D OSX
        INCLUDES += -I../boost_uusi/usr_local_include/
		LIBS     := -L../boost_uusi/usr_local_lib/ -lboost_chrono -lboost_filesystem -lboost_system
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        #CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        #CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        #CCFLAGS += -D ARM
    endif
endif

SRC_DIR := ./src/
OBJECTS  := TextToCppString.o

all: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) -o $(EXECUTABLE)

TextToCppString.o: $(SRC_DIR)TextToCppString.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $(SRC_DIR)TextToCppString.cpp -o TextToCppString.o

clean:
	rm *.o $(EXECUTABLE)

test:
	./text_to_cpp_string ./rae_link_test.hpp 

