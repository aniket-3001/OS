#Compile the launch.c by linking it with the lib_simpleloader.so
# Makefile for compiling launch.c using lib_simpleloader.so

# Compiler settings
CC := gcc
CFLAGS := -I../loader
LDFLAGS := -L../loader -Wl,-rpath=../loader
LDLIBS := ../bin/lib_simpleloader.so

# Source and output files
SRC := launch.c
OUT := launch

# Build rule
all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -m32 -o $@ $< $(LDLIBS)

clean:
	rm ../bin/$(OUT)
#Provide the command for cleanup
