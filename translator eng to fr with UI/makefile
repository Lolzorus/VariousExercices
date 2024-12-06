CPP=g++
EXEC=interlisp
LDFLAGS= -lXm -lXt -lX11
CXXFLAGS= -Wall
SRC= interlisp.cpp
OBJ=$(SRC:.cpp=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CPP) -o $@ $^ $(LDFLAGS)

interlisp.o: interlisp3.cpp
	$(CPP) -o $@ -c $< $(CXXFLAGS)

# pour forcer clean/cleanall même s'il existe un fichier de même nom
.PHONY: clean cleanall

clean:
	rm -rf *.o

cleanall: clean
	rm -rf $(EXEC)
