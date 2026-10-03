CXX      := g++
CXXFLAGS := -O3 -std=c++23 -Wall -Wextra
TARGET   := mem_test

all:
	@rm -f $(TARGET)
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)
	@echo "----------------------------------------"
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
