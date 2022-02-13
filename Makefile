INC_DIR			= include
SRC_DIR			= src
SOURCES			= $(shell find $(SRC_DIR)/ -name '*.cc')
LOC_FLAG 		= $(addprefix -I, $(INC_DIR))
MAIN_SRC		= main.cc
OBJECTS			= $(SOURCES:.cc=.o)
MAIN_OBJ		= $(MAIN_SRC:.cc=.o)
DEPS 			= $(OBJECTS:.o=.d)
MAIN_DEPS		= $(MAIN_OBJ:.o=.d)
MAIN_TARGET		= brutefordle
CC				= clang
CFLAGS			= -std=c++20 -Wall -O2
CLIBS			= -lstdc++ -lm

.PHONY: all clean

all: $(MAIN_TARGET)

$(MAIN_TARGET): $(OBJECTS) $(MAIN_OBJ)
	$(CC) $(LOC_FLAG) $(CFLAGS) $(CLIBS) -o $@ $^
-include $(MAIN_DEPS) $(DEPS)

%.o: %.cc
	$(CC) $(LOC_FLAG) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJECTS) $(MAIN_OBJ) $(DEPS) $(MAIN_DEPS) $(MAIN_TARGET)
