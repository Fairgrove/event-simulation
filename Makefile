CXX = g++
CXXFLAGS = -std=c++17 -Wall
TARGET = main
SRC = main.cpp

run: $(TARGET)
	clear
	./$(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

