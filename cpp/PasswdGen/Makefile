CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = gen.exe
SRCS = PasswdGen.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f *.o $(TARGET)
