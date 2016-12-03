CC = gcc
CXX = g++
CXXFLAGS = -g
CXXFLAGS += -Wall
CXXFLAGS += -Wpedantic
CXXFLAGS += -c
LFLAGS = -lncurses

C_SRC = main.cpp 
C_OBJ = main.o
M_SRCS = Character.cpp Coord.cpp Die.cpp Floor.cpp Game.cpp Item.cpp Space.cpp utils.cpp
M_OBJS = ${M_SRCS:.cpp=.o}

EXEC = vaguely_rogueish 

all: ${EXEC} 

${EXEC}: ${M_OBJS} ${C_OBJ}
	${CXX} $^ -o $@ ${LFLAGS}
	
%.o: %.cpp
	${CXX} ${CXXFLAGS} ${@:.o=.cpp} -o $@

clean:	
	rm -f *.o
	rm -f ${EXEC}
