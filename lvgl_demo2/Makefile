
# Automatically include all source files
SRCS 		:= $(shell find $(SRC_DIR) -type f -name '*.c' -not -path '*/\.*')
# OBJECTS    	:= $(patsubst $(SRC_DIR)%,$(BUILD_DIR)/%,$(SRCS:.$(SRC_EXT)=.$(OBJ_EXT)))

$(PROGRAM): $(SRCS)
	$(CC) $(INCLUDE) -Og $(SRCS) -g $(LDLIBS) -o $(PROGRAM)
clean:
	rm -f $(PROGRAM)
