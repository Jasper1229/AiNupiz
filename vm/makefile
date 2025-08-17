
CFLAGS = -Wall -Wno-incompatible-pointer-types-discards-qualifiers
CPPFLAGS = -Wall -std=c++2a

SRCDIR = .
OBJDIR = ../lib

# Explicitly list all source files instead of using find
C_SRC = main.c \
         vm/vm.c \
         vm/value.c \
         vm/object.c \
         vm/loader.c \
         compiler/chunk.c \
         compiler/compiler.c \
         compiler/dumper.c \
         compiler/scanner.c \
         util/debug.c \
         util/memory.c \
         util/table.c \
         libraries/core/extension.c \
         libraries/core/manager.c \
         libraries/std/nplib.c \
         libraries/fileio/filelib.c \
         libraries/http/httplib.c

CPP_SRC = util/hashing.cpp \
           util/hashvalue.cpp \
           libraries/http/httpclient.cpp

C_OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(C_SRC))
CPP_OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(CPP_SRC))

OBJ = $(C_OBJ) $(CPP_OBJ)

TARGET = ../build/npz

# Create all necessary directories
DIRS = $(OBJDIR) \
       $(OBJDIR)/vm \
       $(OBJDIR)/compiler \
       $(OBJDIR)/util \
       $(OBJDIR)/libraries \
       $(OBJDIR)/libraries/core \
       $(OBJDIR)/libraries/std \
       $(OBJDIR)/libraries/fileio \
       $(OBJDIR)/libraries/http \
       $(OBJDIR)/libraries/vec \
       $(OBJDIR)/libraries/maps \
       ../build

all: $(DIRS) $(TARGET)

rebuild:
	make clean
	make all

$(DIRS):
	mkdir $@

$(TARGET): $(OBJ)
	g++ $(OBJ) -o $@ -lws2_32

$(OBJDIR)/%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
