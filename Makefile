main: main.o lexer.o parser.o runner.o object.o
	g++ -g -Wall $^ -o popcorn

main.o: main.cpp
	g++ -c $<

lexer.o: lib/lexer.cpp
	g++ -c $<

parser.o: lib/parser.cpp
	g++ -c $<

runner.o: lib/runner.cpp
	g++ -c $<

object.o: lib/object.cpp
	g++ -c $<

clean:
	del *.o

clean-wsl:
	rm -rf main *.o