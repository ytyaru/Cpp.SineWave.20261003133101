#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <numbers>
#include <cstdint>
#include <string>

// 1. 【単一責任：データメモリの保持と管理だけを行う】
class SoundBuffer {
private:
    std::vector<float>* buffer; 

public:
    SoundBuffer(size_t total_samples) {
        buffer = new std::vector<float>(total_samples);
    }

    ~SoundBuffer() {
        if (buffer != nullptr) {
            std::cout << "[SoundBuffer RAII] デストラクタ経由で自動解放します。\n";
            free_memory();
        }
    }

    // コピー禁止（二重解放を物理的に防ぐ）
    SoundBuffer(const SoundBuffer&) = delete;
    SoundBuffer& operator=(const SoundBuffer&) = delete;

    // ムーブは許可
    SoundBuffer(SoundBuffer&& other) noexcept : buffer(other.buffer) { other.buffer = nullptr; }
    SoundBuffer& operator=(SoundBuffer&& other) noexcept {
        if (this != &other) {
            if (buffer) delete buffer;
            buffer = other.buffer;
            other.buffer = nullptr;
        }
        return *this;
    }

    // 明示的解放API
    void free_memory() {
        if (buffer != nullptr) {
            delete buffer;
            buffer = nullptr; 
            std::cout << "[SoundBuffer API] 音声メモリを明示的に完全解放しました。\n";
        }
    }

    float* data() { return buffer ? buffer->data() : nullptr; }
    const float* data() const { return buffer ? buffer->data() : nullptr; }
    size_t size() const { return buffer ? buffer->size() : 0; }
};

// 2. 【単一責任：ファイル操作（ディスク書き出し）だけを担うクラス】
class WavFileWriter {
private:
    // 指摘の通り、WavHeader構造体をこのクラス内専用（private）にカプセル化
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
        uint16_t bits_per_sample = 16; // 16bitで書き出す
        char data_id[4] = {'d', 'a', 't', 'a'};
        uint32_t data_size;
    };
    #pragma pack(pop)

public:
    // メモリ（SoundBuffer）を受け取り、それをWAVファイルに変換してディスクに保存する
    static void save(const std::string& filename, const SoundBuffer& sound_buf, uint32_t sample_rate) {
        if (sound_buf.data() == nullptr || sound_buf.size() == 0) {
            std::cerr << "[WavFileWriter] メモリが空のためファイル出力できません。\n";
            return;
        }

        WavHeader header;
        header.sample_rate = sample_rate;
        header.data_size = static_cast<uint32_t>(sound_buf.size() * sizeof(int16_t));
        header.file_size = header.data_size + 36;
        header.block_align = static_cast<uint16_t>(header.num_channels * (header.bits_per_sample / 8));
        header.byte_rate = header.sample_rate * header.block_align;

        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) {
            std::cerr << "[WavFileWriter] ファイルを作成できませんでした。\n";
            return;
        }

        ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
        
        // メモリ内のデータ（float）を取り出して、ファイル用の16bit整数に変換しながら書き込む
        const float* raw_data = sound_buf.data();
        for (size_t i = 0; i < sound_buf.size(); ++i) {
            int16_t pcm_sample = static_cast<int16_t>(raw_data[i] * 32767.0f);
            ofs.write(reinterpret_cast<const char*>(&pcm_sample), sizeof(int16_t));
        }
        std::cout << "[WavFileWriter API] ディスクに " << filename << " を書き出しました。\n";
    }
};

// 3. 【単一責任：音波の数式計算だけを行うクラス】
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

int main() {
    const uint32_t sample_rate = 44100;
    SoundGenerator generator(sample_rate);

    std::cout << "--- 実験1: メモリ生成 ➡ 外部ライターによるWAV保存 ➡ 明示的解放 ---\n";
    {
        SoundBuffer wav1 = generator.create_sine(440.0f, 1.5f, 0.5f);
        WavFileWriter::save("output_mem.wav", wav1, sample_rate);
        wav1.free_memory();
    } 

    std::cout << "\n--- 実験2: 消し忘れてもRAIIで自動解放 ---\n";
    {
        SoundBuffer wav2 = generator.create_sine(523.25f, 1.0f, 0.5f);
        WavFileWriter::save("output_raii.wav", wav2, sample_rate);
        
        std::cout << "★明示的解放を呼ばずにスコープを抜けます...\n";
    } // デストラクタ経由で自動的にfree_memory()が呼ばれる

    return 0;
} // 欠落していたmain関数の閉じカッコを完全に補完
