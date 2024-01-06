main: main.o lexer.o parser.o runner.o object.o ast.o console.o
	g++ -g -Wall $^ -o popcorn

main.o: main.cpp
	g++ -c $<

lexer.o: lib/lexer.cpp lib/lexer.hpp
	g++ -c $<

parser.o: lib/parser.cpp lib/parser.hpp
	g++ -c $<

runner.o: lib/runner.cpp lib/runner.hpp
	g++ -c $<

object.o: lib/object.cpp lib/object.hpp
	g++ -c $<

ast.o: lib/ast.cpp lib/ast.hpp
	g++ -c $<

console.o: lib/console.cpp lib/console.hpp
	g++ -c $<

clean:
	del *.o

clean-wsl:
	rm -rf main *.o