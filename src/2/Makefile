CXX      := g++
CXXFLAGS := -O3 -std=c++23 -Wall -Wextra
TARGET   := separate_test

all:
	@rm -f $(TARGET) *.wav
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)
	@echo "----------------------------------------"
	./$(TARGET)
	@echo "----------------------------------------"
	@echo "【再生】1つ目のWAV（明示的解放されたバッファから出力した音）..."
	aplay output_mem.wav
	@echo "【再生】2つ目のWAV（RAII自動連鎖解放で出力した音）..."
	aplay output_raii.wav

clean:
	rm -f $(TARGET) *.wav

.PHONY: all clean
