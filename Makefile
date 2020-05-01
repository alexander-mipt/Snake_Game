ALL = main
OBJS = snake.o ui.o tui.o sfm.o control.o main.o

#CXXFLAGS += -O2
CXXFLAGS += -std=c++17 -Wall

all:	$(ALL)

main:	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

clean:
	rm -fv *.o $(OBJS) $(ALL)

depend:
	$(CXX) $(CXXFLAGS) -MM $(OBJS:.o=.cpp) >.depend

-include .depend
