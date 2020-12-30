cflags= -g -Wall -std=c99 -pedantic -lm

compile = gcc

program = shrink

class = graph

csources = $(class).h $(program).c 

cobjects = $(program).o

$(program): $(cobjects)
	$(compile) -o $(program) $(cobjects) $(cflags)

$(cobjects): $(csources)
	$(compile)  $^ -c

clean:
	rm -r $(program).o
	rm -r $(program)
	rm -r $(cobjects)
	rm -r *.h.gch

