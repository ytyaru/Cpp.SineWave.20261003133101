# コンパイラとフラグ
CXX      := g++
CXXFLAGS := -O3 -std=c++23 -Wall -Wextra

TARGET   := wav_generator
WAV_FILE := output.wav

# 【修正】make と打つだけで、強制再ビルド → 音声生成 → 再生 まで一括で実行します
all:
	@echo "[1/3] 古いファイルを削除して強制的に再コンパイルします..."
	@rm -f $(TARGET) $(WAV_FILE)
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)
	@echo "[2/3] サイン波（output.wav）を生成中..."
	./$(TARGET)
	@echo "[3/3] ラズパイのスピーカーから再生します..."
	aplay $(WAV_FILE)

clean:
	rm -f $(TARGET) $(WAV_FILE)

.PHONY: all clean
