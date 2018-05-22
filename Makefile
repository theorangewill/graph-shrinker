cflags= -c -g -Wall -std=c99 -pedantic -lm

compile = gcc

program = contrair

class = grafo

csources = $(class).h $(program).c 

cobjects = $(program).o

$(program): $(cobjects)
	$(compile) -o $(program) $(cobjects)

$(cobjects): $(csources)
	$(compile) $^ $(cflags)

clean:
	rm -r $(program)
	rm -r $(cobjects)
	rm -r *.h.gch

