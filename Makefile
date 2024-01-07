CC = g++
CFLAGS = -g -Wall

ifeq ($(origin OS), undefined)
	CLEAN = rm -rf main *.o	
else
	CLEAN = del *.o
endif

# ifeq ($(PLATFORM), Linux)
# 	CLEAN = rm -rf main *.o	
# else
# 	CLEAN = del *.o
# endif

main: main.o lexer.o parser.o runner.o object.o ast.o console.o
	$(CC) $(CFLAGS) $^ -o popcorn

main.o: main.cpp
	$(CC) -c $<

lexer.o: lib/lexer.cpp lib/lexer.hpp
	$(CC) -c $<

parser.o: lib/parser.cpp lib/parser.hpp
	$(CC) -c $<

runner.o: lib/runner.cpp lib/runner.hpp
	$(CC) -c $<

object.o: lib/object.cpp lib/object.hpp
	$(CC) -c $<

ast.o: lib/ast.cpp lib/ast.hpp
	$(CC) -c $<

console.o: lib/console.cpp lib/console.hpp
	$(CC) -c $<

clean:
	$(CLEAN)