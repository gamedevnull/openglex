CXX = g++
CXXFLAGS = -std=c++11 -Wall
LIBS = -I include -L /usr/local/lib -lSDL2 -framework OpenGL
SRCS = main.cpp
TARGET = main.out
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)