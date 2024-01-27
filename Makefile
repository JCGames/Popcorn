CC = g++
CFLAGS = -g -Wall

ifeq ($(origin OS), undefined)
	CLEAN = rm -rf lib/*.o popcorn *.o includes/console.o
else
	CLEAN = cd lib && del *.o && cd .. && del *.exe *.o && cd includes && del console.o && cd ..
endif

CPP_FILES := $(wildcard lib/*.cpp)
OBJ_FILES := $(patsubst %.cpp, %.o, $(CPP_FILES))

popcorn: main.o $(OBJ_FILES) includes/console.o
	$(CC) $(CFLAGS) $^ -o popcorn

main.o: main.cpp
	$(CC) -c $(CFLAGS) $<

%.o: lib/%.cpp
	$(CC) -c $(CFLAGS) $< -o $o

console.o: includes/console.cpp
	$(CC) -c $(CFLAGS) $<

clean:
	$(CLEAN)