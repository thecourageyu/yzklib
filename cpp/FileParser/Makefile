CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = fparser.exe
SRCS = main.cpp FileParser.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f *.o $(TARGET)
