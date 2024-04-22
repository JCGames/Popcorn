
main: main.o file.o diagnostics.o tokenizer.o parser.o runner.o object.o
	g++ $^ -o pop

main.o: main.cpp
	g++ -c $<

file.o: file.cpp file.hpp 
	g++ -c $<

diagnostics.o: diagnostics.cpp diagnostics.hpp 
	g++ -c $<

tokenizer.o: tokenizer.cpp tokenizer.hpp 
	g++ -c $<

parser.o: parser.cpp parser.hpp 
	g++ -c $<

runner.o: runner.cpp runner.hpp
	g++ -c $<

object.o: object.cpp object.hpp
	g++ -c $<

clean:
	del *.o *.exe pop