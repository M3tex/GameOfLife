SRC := ./src
BUILD := ./build
INCLUDE := ./include

# '-I .' pour spécifier où sont les headers
C_FLAGS := -I $(INCLUDE) -Wall -g


C_FILES := $(wildcard $(SRC)/*.c)
HEADERS := $(wildcard $(INCLUDE)/*.h)
OBJS := $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(C_FILES))


linker_SDL := `sdl2-config --cflags --libs`

all: $(OBJS) $(HEADERS) $(C_FILES) gol


$(BUILD)/%.o: $(SRC)/%.c 
	gcc -c -c $< -o $@ $(C_FLAGS)

gol:
	gcc -o gol $(OBJS) $(linker_SDL) $(C_FLAGS) -lm


clean:
	rm ./build/*
	rm gol
