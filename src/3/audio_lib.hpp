#ifndef AUDIO_LIB_HPP
#define AUDIO_LIB_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <numbers>
#include <cstdint>
#include <string>
#include <cstring> // std::memcpy のために必須

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

// 1. 裸の浮動小数点音声データのメモリ保持と管理だけを行うクラス
class SoundBuffer {
private:
    std::vector<float>* buffer; 

public:
    SoundBuffer(size_t total_samples) {
        buffer = new std::vector<float>(total_samples);
    }

    ~SoundBuffer() {
        if (buffer != nullptr) {
            std::cout << "[SoundBuffer RAII] 自動解放します。\n";
            free_memory();
        }
    }

    SoundBuffer(const SoundBuffer&) = delete;
    SoundBuffer& operator=(const SoundBuffer&) = delete;

    SoundBuffer(SoundBuffer&& other) noexcept : buffer(other.buffer) { other.buffer = nullptr; }
    SoundBuffer& operator=(SoundBuffer&& other) noexcept {
        if (this != &other) {
            if (buffer) delete buffer;
            buffer = other.buffer;
            other.buffer = nullptr;
        }
        return *this;
    }

    void free_memory() {
        if (buffer != nullptr) {
            delete buffer;
            buffer = nullptr; 
            std::cout << "[SoundBuffer API] メモリを完全解放しました。\n";
        }
    }

    float* data() { return buffer ? buffer->data() : nullptr; }
    const float* data() const { return buffer ? buffer->data() : nullptr; }
    size_t size() const { return buffer ? buffer->size() : 0; }
};

// 2. WAVフォーマットに沿った完成済みのバイナリをメモリ上に構築・保持するクラス
class WavFileBuffer {
private:
    std::vector<uint8_t>* wav_bytes; // WAV全体のバイト配列ポインタ

    #pragma pack(push, 1)
    struct WavHeader {
        char riff_id[4] = {'R', 'I', 'F', 'F'};
        uint32_t file_size;
        char wave_id[4] = {'W', 'A', 'V', 'E'};
        char fmt_id[4] = {'f', 'm', 't', ' '};
        uint32_t fmt_size = 16;
        uint16_t audio_format = 1;     // 1 = 整数リニアPCM
        uint16_t num_channels = 1;     // 1 = モノラル
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample = 16; // 16bit
        char data_id[4] = {'d', 'a', 't', 'a'};
        uint32_t data_size;
    };
    #pragma pack(pop)

public:
    WavFileBuffer(const SoundBuffer& sound_buf, uint32_t sample_rate) {
        size_t data_size_bytes = sound_buf.size() * sizeof(int16_t);
        size_t total_file_size_bytes = sizeof(WavHeader) + data_size_bytes;

        wav_bytes = new std::vector<uint8_t>(total_file_size_bytes);

        WavHeader header;
        header.sample_rate = sample_rate;
        header.data_size = static_cast<uint32_t>(data_size_bytes);
        header.file_size = header.data_size + 36;
        header.block_align = static_cast<uint16_t>(header.num_channels * (header.bits_per_sample / 8));
        header.byte_rate = header.sample_rate * header.block_align;

        uint8_t* head_ptr = wav_bytes->data();
        std::memcpy(head_ptr, &header, sizeof(WavHeader));

        int16_t* pcm_ptr = reinterpret_cast<int16_t*>(head_ptr + sizeof(WavHeader));
        const float* raw_data = sound_buf.data();

        for (size_t i = 0; i < sound_buf.size(); ++i) {
            pcm_ptr[i] = static_cast<int16_t>(raw_data[i] * 32767.0f);
        }
        std::cout << "[WavFileBuffer] メモリ上にWAVフォーマットのバイナリを構築しました。\n";
    }

    ~WavFileBuffer() {
        if (wav_bytes != nullptr) {
            std::cout << "[WavFileBuffer RAII] 自動解放します。\n";
            free_memory();
        }
    }

    WavFileBuffer(const WavFileBuffer&) = delete;
    WavFileBuffer& operator=(const WavFileBuffer&) = delete;

    WavFileBuffer(WavFileBuffer&& other) noexcept : wav_bytes(other.wav_bytes) { other.wav_bytes = nullptr; }
    WavFileBuffer& operator=(WavFileBuffer&& other) noexcept {
        if (this != &other) {
            if (wav_bytes) delete wav_bytes;
            wav_bytes = other.wav_bytes;
            other.wav_bytes = nullptr; // タイポを wav_bytes に修正
        }
        return *this;
    }

    void free_memory() {
        if (wav_bytes != nullptr) {
            delete wav_bytes;
            wav_bytes = nullptr;
            std::cout << "[WavFileBuffer API] WAVバイトメモリを明示的に完全解放しました。\n";
        }
    }

    const uint8_t* data() const { return wav_bytes ? wav_bytes->data() : nullptr; }
    size_t size() const { return wav_bytes ? wav_bytes->size() : 0; }
};

// 3. 完成したWavFileBufferのメモリ内容をディスクに書き出すだけを行うクラス
class WavFileWriter {
public:
    static void save(const std::string& filename, const WavFileBuffer& wav_buf) {
        if (wav_buf.data() == nullptr || wav_buf.size() == 0) {
            std::cerr << "[WavFileWriter] WAVバッファが空のためファイル出力できません。\n";
            return;
        }

        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) {
            std::cerr << "[WavFileWriter] ファイルを作成できませんでした。\n";
            return;
        }

        ofs.write(reinterpret_cast<const char*>(wav_buf.data()), wav_buf.size());
        std::cout << "[WavFileWriter API] ディスクに " << filename << " を書き出しました。\n";
    }
};

// 4. 音波の数式計算だけを行うクラス
class SoundGenerator {
private:
    uint32_t sample_rate;

public:
    SoundGenerator(uint32_t sample_rate) : sample_rate(sample_rate) {}

    SoundBuffer create_sine(float frequency, float duration, float volume) {
        size_t total_samples = static_cast<size_t>(static_cast<double>(sample_rate) * duration);
        SoundBuffer sound_buf(total_samples);
        float* raw_ptr = sound_buf.data();

        for (size_t i = 0; i < total_samples; ++i) {
            double time = static_cast<double>(i) / static_cast<double>(sample_rate);
            double radian = 2.0 * std::numbers::pi * frequency * time;
            raw_ptr[i] = static_cast<float>(std::sin(radian) * volume);
        }

        return sound_buf; 
    }
};

#endif
