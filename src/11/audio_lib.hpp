#ifndef AUDIO_LIB_HPP
#define AUDIO_LIB_HPP

#include <vector>
#include <cmath>
#include <cstdint>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace AudioDefaults {
    constexpr uint32_t SAMPLE_RATE = 44100;
    constexpr float FREQUENCY = 440.0f;
    constexpr float DURATION = 3.0f;
    constexpr float VOLUME = 0.5f;
}

class SoundBuffer {
private:
    std::vector<float> buffer;

public:
    SoundBuffer() = default;
    explicit SoundBuffer(size_t total_samples) : buffer(total_samples) {}

    // 以下の operator[] を追加
    float& operator[](size_t index) { return buffer[index]; }
    const float& operator[](size_t index) const { return buffer[index]; }

    float* data() { return buffer.data(); }
    const float* data() const { return buffer.data(); }
    size_t size() const { return buffer.size(); }
    bool empty() const { return buffer.empty(); }
};
class WavFileBuffer {
private:
    #pragma pack(push, 1)
    struct WavHeader {
        char riff_id[4] = {'R', 'I', 'F', 'F'};
        uint32_t file_size;
        char wav_id[4] = {'W', 'A', 'V', 'E'};
        char fmt_id[4] = {'f', 'm', 't', ' '};
        uint32_t fmt_size = 16;
        uint16_t audio_format = 1;
        uint16_t num_channels = 1;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample = 16;
        char data_id[4] = {'d', 'a', 't', 'a'};
        uint32_t data_size;
    };
    #pragma pack(pop)

    std::vector<uint8_t> binary_data;

public:
    WavFileBuffer() = default;

    WavFileBuffer(const SoundBuffer& sound_buf, uint32_t sample_rate) {
        uint32_t data_size_bytes = static_cast<uint32_t>(sound_buf.size() * sizeof(int16_t));
        
        WavHeader header;
        header.sample_rate = sample_rate;
        header.data_size = data_size_bytes;
        header.file_size = sizeof(WavHeader) + data_size_bytes - 8;
        header.byte_rate = sample_rate * 1 * (16 / 8);
        header.block_align = 1 * (16 / 8);

        binary_data.resize(sizeof(WavHeader) + data_size_bytes);
        std::memcpy(binary_data.data(), &header, sizeof(WavHeader));

        int16_t* pcm_ptr = reinterpret_cast<int16_t*>(binary_data.data() + sizeof(WavHeader));
        for (size_t i = 0; i < sound_buf.size(); ++i) {
            float sample = sound_buf[i];
            if (sample > 1.0f) sample = 1.0f;
            if (sample < -1.0f) sample = -1.0f;
            pcm_ptr[i] = static_cast<int16_t>(sample * 32767.0f);
        }
    }

    // cli.cpp が呼び出している empty() メソッドを復元
    bool empty() const {
        return binary_data.empty();
    }

    const uint8_t* data() const { return binary_data.data(); }
    size_t size() const { return binary_data.size(); }
};

class SoundGenerator {
private:
    uint32_t sample_rate;

public:
    explicit SoundGenerator(uint32_t sample_rate = AudioDefaults::SAMPLE_RATE) : sample_rate(sample_rate) {}

    SoundBuffer create_sine(float frequency = AudioDefaults::FREQUENCY, 
                            float duration = AudioDefaults::DURATION, 
                            float volume = AudioDefaults::VOLUME) {
        size_t total_samples = static_cast<size_t>(static_cast<double>(sample_rate) * duration);
        SoundBuffer sound_buf(total_samples);
        float* raw_ptr = sound_buf.data();

        for (size_t i = 0; i < total_samples; ++i) {
            double time = static_cast<double>(i) / static_cast<double>(sample_rate);
            double radian = 2.0 * M_PI * frequency * time;
            raw_ptr[i] = static_cast<float>(std::sin(radian) * volume);
        }

        return sound_buf;
    }
};

#endif
