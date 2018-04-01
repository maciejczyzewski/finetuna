CC := gcc-7
CFLAGS := -g -Wall
LIBS := -lallegro -lallegro_main -lallegro_dialog -lallegro_primitives
SOURCES := $(shell find src -type f -name "*.c")
HEADERS := $(shell find src -type f -name "*.h")
OBJECTS := $(SOURCES:.c=.o)
TARGET := Finetuna

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "\033[92m[+]\033[m Linking..."; $(CC) $^ -o $@ $(LIBS)

%.o: %.c $(HEADERS)
	@echo "\033[92m[+]\033[m CC $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "\033[92m[+]\033[m Cleaning..."; $(RM) src/*.o $(TARGET)

.PHONY: all clean
