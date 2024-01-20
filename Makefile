CC = g++
CFLAGS = -g -Wall

ifeq ($(origin OS), undefined)
	CLEAN = rm -rf lib/*.o popcorn *.o
else
	CLEAN = cd lib && del *.o && cd .. && del *.exe *.o
endif

CPP_FILES := $(wildcard lib/*.cpp)
OBJ_FILES := $(patsubst %.cpp, %.o, $(CPP_FILES))

popcorn: main.o $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o popcorn

main.o: main.cpp
	$(CC) -c $(CFLAGS) $<

%.o: lib/%.cpp
	$(CC) -c $(CFLAGS) $< -o $o

clean:
	$(CLEAN)