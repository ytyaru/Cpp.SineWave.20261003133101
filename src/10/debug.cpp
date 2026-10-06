#include "audio_lib.hpp"
#include <iostream>
#include <fstream>
#include <string>

class WavFileWriter {
public:
    static bool save(const std::string& filename, const WavFileBuffer& wav_buf) {
        if (wav_buf.empty()) return false;
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) return false;
        ofs.write(reinterpret_cast<const char*>(wav_buf.data()), wav_buf.size());
        std::cout << "[WavFileWriter] ディスクに " << filename << " を書き出しました。\n";
        return true;
    }
};

int main() {
    std::cout << "=== 音声ライブラリ 動作確認デバッグ ===\n";

    try {
        std::cout << "[1] サイン波を生成しています (440Hz, 2.0秒, 音量0.5)...\n";
        SoundGenerator generator(AudioDefaults::SAMPLE_RATE);
        SoundBuffer raw_sound = generator.create_sine(440.0f, 2.0f, 0.5f);
        std::cout << "    -> 生成サンプル数: " << raw_sound.size() << " 成功\n";

        std::cout << "[2] WAVファイル形式に変換しています...\n";
        WavFileBuffer wav_file(raw_sound, AudioDefaults::SAMPLE_RATE);
        std::cout << "    -> WAVバイナリサイズ: " << wav_file.size() << " bytes 成功\n";

        // Makefile が期待している output_mem.wav に一致させる
        //std::string filename = "output_mem.wav";
        std::string filename = "output.wav";
        std::cout << "[3] ファイルに保存しています: " << filename << " ...\n";
        if (WavFileWriter::save(filename, wav_file)) {
            std::cout << "    -> 保存完了！正常に音声データが出力されました。\n";
        } else {
            std::cerr << "    -> [Error] ファイルの保存に失敗しました。\n";
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "[Error] 例外が発生しました: " << e.what() << "\n";
        return 1;
    }

    std::cout << "\nすべてのデバッグ処理が正常に終了しました。\n";
    return 0;
}
