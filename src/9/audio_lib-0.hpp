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

    float* data() { return buffer.data(); }
    const float* data() const { return buffer.data(); }
    size_t size() const { return buffer.size(); }
    bool empty() const { return buffer.empty(); }
};

class WavFileBuffer {
private:
    std::vector<uint8_t> wav_bytes;

    #pragma pack(push, 1)
    struct WavHeader {
        char riff_id[4];
        uint32_t file_size;
        char wave_id[4];
        char fmt_id[4];
        uint32_t fmt_size;
        uint16_t audio_format;
        uint16_t num_channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample;
        char data_id[4];
        uint32_t data_size;

        WavHeader() {
            riff_id[0] = 'R'; riff_id[1] = 'I'; riff_id[2] = 'F'; riff_id[3] = 'F';
            wave_id[0] = 'W'; wave_id[1] = 'A'; wave_id[2] = 'V'; wave_id[3] = 'E';
            fmt_id[0] = 'f';  fmt_id[1] = 'm';  fmt_id[2] = 't';  fmt_id[3] = ' ';
            fmt_size = 16;
            audio_format = 1;
            num_channels = 1;
            bits_per_sample = 16;
            data_id[0] = 'd'; data_id[1] = 'a'; data_id[2] = 't'; data_id[3] = 'a';
        }
    };
    #pragma pack(pop)

public:
    WavFileBuffer() = default;

    WavFileBuffer(const SoundBuffer& sound_buf, uint32_t sample_rate) {
        size_t data_size_bytes = sound_buf.size() * sizeof(int16_t);
        size_t total_file_size_bytes = sizeof(WavHeader) + data_size_bytes;

        wav_bytes.resize(total_file_size_bytes);

        WavHeader header;
        header.sample_rate = sample_rate;
        header.data_size = static_cast<uint32_t>(data_size_bytes);
        header.file_size = header.data_size + 36;
        header.block_align = static_cast<uint16_t>(header.num_channels * (header.bits_per_sample / 8));
        header.byte_rate = header.sample_rate * header.block_align;

        uint8_t* head_ptr = wav_bytes.data();
        std::memcpy(head_ptr, &header, sizeof(WavHeader));

        int16_t* pcm_ptr = reinterpret_cast<int16_t*>(head_ptr + sizeof(WavHeader));
        const float* raw_data = sound_buf.data();

        for (size_t i = 0; i < sound_buf.size(); ++i) {
            pcm_ptr[i] = static_cast<int16_t>(raw_data[i] * 32767.0f);
        }
    }

    const uint8_t* data() const { return wav_bytes.data(); }
    size_t size() const { return wav_bytes.size(); }
    bool empty() const { return wav_bytes.empty(); }
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
