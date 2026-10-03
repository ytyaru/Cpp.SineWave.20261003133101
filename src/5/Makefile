CXX      := g++
CXXFLAGS := -O3 -std=c++17 -Wall -Wextra
TARGET   := separate_test

all: clean
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)
	@echo "----------------------------------------"
	./$(TARGET)
	@echo "----------------------------------------"
	@echo "【再生】1つ目のWAV..."
	aplay output_mem.wav
	@echo "【再生】2つ目のWAV..."
	aplay output_raii.wav

clean:
	rm -f $(TARGET) *.wav

.PHONY: all clean
