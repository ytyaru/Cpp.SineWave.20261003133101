# 自動的に2コアを使用して並列ビルドする設定
MAKEFLAGS += -j2

CXX      := g++
CXXFLAGS := -O3 -std=c++17 -Wall -Wextra

TARGET_CLI   := sine
TARGET_DEBUG := debug
OUTPUT_WAV   := output.wav
TARGET_WASM  := audio_wasm.js

all: $(TARGET_CLI) $(TARGET_DEBUG)
	@echo "----------------------------------------"
	./$(TARGET_DEBUG)
	@echo "----------------------------------------"
	@echo "【再生】WAVファイル..."
	aplay $(OUTPUT_WAV) || true
	@echo "----------------------------------------"
	@echo "【WASMビルドを実行します】"
	@which emcc >/dev/null 2>&1 || (echo "エラー: emcc (Emscripten) がインストールされていません、またはパスが通っていません。" && exit 1)
	$(MAKE) wasm

# 1. 本番用CLIのビルド
$(TARGET_CLI): cli.cpp audio_lib.hpp
	$(CXX) $(CXXFLAGS) cli.cpp -o $(TARGET_CLI)

# 2. デバッグ・動作確認用バイナリのビルド
$(TARGET_DEBUG): debug.cpp audio_lib.hpp
	$(CXX) $(CXXFLAGS) debug.cpp -o $(TARGET_DEBUG)

# 3. WASM一括ビルド
wasm: audio_wasm_esm.js audio_wasm_nomodule.js

# 3-1. ESM 版のビルド (-sMODULARIZE=1 -sEXPORT_ES6=1 -sEXPORTED_RUNTIME_METHODS を追加)
audio_wasm_esm.js: wasm_api.cpp audio_lib.hpp
	em++ -O3 -std=c++17 -frtti --bind -s ALLOW_MEMORY_GROWTH=1 -fexceptions \
		-sMODULARIZE=1 -sEXPORT_ES6=1 -sEXPORTED_RUNTIME_METHODS=['HEAPU8'] \
		wasm_api.cpp -o audio_wasm_esm.js

# 3-2. Classic (No-Module) 版のビルド (-sMODULARIZE=1 -sEXPORT_NAME='createAudioModule' -sEXPORTED_RUNTIME_METHODS を追加)
audio_wasm_nomodule.js: wasm_api.cpp audio_lib.hpp
	em++ -O3 -std=c++17 -frtti --bind -s ALLOW_MEMORY_GROWTH=1 -fexceptions \
		-sMODULARIZE=1 -sEXPORT_NAME='createAudioModule' -sEXPORTED_RUNTIME_METHODS=['HEAPU8'] \
		wasm_api.cpp -o audio_wasm_nomodule.js

clean:
	rm -f $(TARGET_CLI) $(TARGET_DEBUG) *.wav audio_wasm*.js audio_wasm*.wasm

.PHONY: all wasm clean
