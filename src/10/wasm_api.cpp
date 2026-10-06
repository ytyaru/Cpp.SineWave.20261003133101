#include "audio_lib.hpp"
#include <memory>
#include <stdexcept>
#include <cstdint>

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

class SineWaveManager {
private:
    std::unique_ptr<WavFileBuffer> current_wav;

public:
    SineWaveManager() = default;

    // voidからemscripten::valに変更
    emscripten::val generate(uint32_t sample_rate, float frequency, float duration, float volume) {
        SoundGenerator generator(sample_rate);
        SoundBuffer raw = generator.create_sine(frequency, duration, volume);
        current_wav = std::make_unique<WavFileBuffer>(raw, sample_rate);
        // 以下を追記
        return get_uint8_array();
    }

    // C++ から直接 JavaScript の Uint8Array を生成して返却する
    emscripten::val get_uint8_array() const {
        if (!current_wav || current_wav->empty()) {
            throw std::runtime_error("WAV buffer is not generated yet.");
        }
        
        // JSの Uint8Array コンストラクタを呼び出し、WASM上のメモリからコピーを生成
        return emscripten::val(emscripten::typed_memory_view(
            current_wav->size(),
            current_wav->data()
        ));
    }
    /*
    // ポインタを直接返さず、メモリアドレス（整数）を返す
    uintptr_t get_data() const {
        if (!current_wav) {
            throw std::runtime_error("WAV buffer is not generated yet.");
        }
        return reinterpret_cast<uintptr_t>(current_wav->data());
    }
    */
    size_t get_size() const {
        return current_wav ? current_wav->size() : 0;
    }

    void clear() {
        current_wav.reset();
    }
};

#ifdef __EMSCRIPTEN__
using namespace emscripten;

EMSCRIPTEN_BINDINGS(AudioModule) {
    class_<SineWaveManager>("SineWaveManager")
        .constructor<>()
        .function("generate", &SineWaveManager::generate)
//        .function("get_data", &SineWaveManager::get_data) // allow_raw_pointers() も不要になります
        .function("get_uint8_array", &SineWaveManager::get_uint8_array) // allow_raw_pointers() も不要になります
        .function("get_size", &SineWaveManager::get_size)
        .function("clear", &SineWaveManager::clear);
}
#endif
