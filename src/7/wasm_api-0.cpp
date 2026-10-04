#include "audio_lib.hpp"
#include <memory>
#include <stdexcept>

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

// あなたの提案したファサード設計を反映したC++側マネージャー
class SineWaveManager {
private:
    std::unique_ptr<WavFileBuffer> current_wav;

public:
    SineWaveManager() = default;

    // 生成メソッド：例外が発生した場合はそのままスローし、Emscripten経由でJSの catch (e) に伝播させる
    void generate(uint32_t sample_rate, float frequency, float duration, float volume) {
        SoundGenerator generator(sample_rate);
        SoundBuffer raw = generator.create_sine(frequency, duration, volume);
        current_wav = std::make_unique<WavFileBuffer>(raw, sample_rate);
    }

    const uint8_t* get_data() const {
        if (!current_wav) {
            throw std::runtime_error("WAV buffer is not generated yet.");
        }
        return current_wav->data();
    }

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
        .function("get_data", &SineWaveManager::get_data, allow_raw_pointers())
        .function("get_size", &SineWaveManager::get_size)
        .function("clear", &SineWaveManager::clear);
}
#endif
