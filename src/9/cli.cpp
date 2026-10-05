#include "audio_lib.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

class WavFileWriter {
public:
    static bool save(const std::string& filename, const WavFileBuffer& wav_buf) {
        if (wav_buf.empty()) return false;

        if (filename == "-") {
#ifdef _WIN32
            _setmode(_fileno(stdout), _O_BINARY);
#endif
            std::cout.write(reinterpret_cast<const char*>(wav_buf.data()), wav_buf.size());
            return true;
        }

        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) return false;

        ofs.write(reinterpret_cast<const char*>(wav_buf.data()), wav_buf.size());
        return true;
    }
};

void print_help() {
    std::cout << "sine - サイン波WAV音声生成ツール\n\n";
    std::cout << "使い方:\n";
    std::cout << "  sine [オプション]\n\n";
    std::cout << "オプション:\n";
    std::cout << "  -r, --rate <Hz>       サンプリングレート (デフォルト: " << AudioDefaults::SAMPLE_RATE << ")\n";
    std::cout << "  -f, --frequency <Hz>  周波数 (デフォルト: " << AudioDefaults::FREQUENCY << ")\n";
    std::cout << "  -d, --duration <秒>   再生時間 (デフォルト: " << AudioDefaults::DURATION << ")\n";
    std::cout << "  -v, --volume <0.0-1.0> 音量 (デフォルト: " << AudioDefaults::VOLUME << ")\n";
    std::cout << "  -o, --output <path>   出力ファイルパス ('-' で標準出力) (デフォルト: -)\n";
    std::cout << "  -h, --help            このヘルプを表示する\n";
}

int main(int argc, char* argv[]) {
    uint32_t sample_rate = AudioDefaults::SAMPLE_RATE;
    float frequency = AudioDefaults::FREQUENCY;
    float duration = AudioDefaults::DURATION;
    float volume = AudioDefaults::VOLUME;
    std::string output_path = "-";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        } else if ((arg == "-r" || arg == "--rate") && i + 1 < argc) {
            sample_rate = static_cast<uint32_t>(std::stoi(argv[++i]));
        } else if ((arg == "-f" || arg == "--frequency") && i + 1 < argc) {
            frequency = std::stof(argv[++i]);
        } else if ((arg == "-d" || arg == "--duration") && i + 1 < argc) {
            duration = std::stof(argv[++i]);
        } else if ((arg == "-v" || arg == "--volume") && i + 1 < argc) {
            volume = std::stof(argv[++i]);
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            output_path = argv[++i];
        }
    }

    try {
        SoundGenerator generator(sample_rate);
        SoundBuffer raw_sound = generator.create_sine(frequency, duration, volume);
        WavFileBuffer wav_file(raw_sound, sample_rate);

        if (!WavFileWriter::save(output_path, wav_file)) {
            std::cerr << "[Error] 出力の保存に失敗しました: " << output_path << "\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "[Error] 例外が発生しました: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
