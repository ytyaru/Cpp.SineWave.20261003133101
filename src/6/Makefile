CXX      := g++
CXXFLAGS := -O3 -std=c++17 -Wall -Wextra

TARGET_CLI   := sine
TARGET_DEBUG := separate_test

all: $(TARGET_CLI) $(TARGET_DEBUG)
	@echo "----------------------------------------"
	./$(TARGET_DEBUG)
	@echo "----------------------------------------"
	@echo "【再生】WAVファイル..."
	aplay output_mem.wav || true

$(TARGET_CLI): cli.cpp audio_lib.hpp
	$(CXX) $(CXXFLAGS) cli.cpp -o $(TARGET_CLI)

$(TARGET_DEBUG): debug.cpp audio_lib.hpp
	$(CXX) $(CXXFLAGS) debug.cpp -o $(TARGET_DEBUG)

clean:
	rm -f $(TARGET_CLI) $(TARGET_DEBUG) *.wav

.PHONY: all clean
