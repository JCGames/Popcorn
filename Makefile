main: main.o lexer.o parser.o runner.o
	g++ -g -Wall $^ -o main

main.o: main.cpp
	g++ -c $<

lexer.o: lexer.cpp
	g++ -c $<

parser.o: parser.cpp
	g++ -c $<

runner.o: runner.cpp
	g++ -c $<

clean:
	del *.exe *.o main

clean-wsl:
	rm -rf main *.o *.exe