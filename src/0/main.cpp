#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <numbers>
#include <cstdint>
#include <string>

// WAVファイルの先頭に書き込む44バイトのヘッダー構造体
// コンパイラによるパディング（隙間あけ）を防ぐため、厳密なサイズで配置します
#pragma pack(push, 1)
struct WavHeader {
    // RIFF チャンク記述子
    char riff_id[4] = {'R', 'I', 'F', 'F'};
    uint32_t file_size;                 // これ以降の全ファイルサイズ (データサイズ + 36)
    char wave_id[4] = {'W', 'A', 'V', 'E'};

    // fmt チャンク (フォーマット情報)
    char fmt_id[4] = {'f', 'm', 't', ' '};
    uint32_t fmt_size = 16;             // リニアPCMの場合は 16 固定
    uint16_t audio_format = 1;          // 1 = 整数リニアPCM (16bit)
    uint16_t num_channels = 1;          // 1 = モノラル
    uint32_t sample_rate;               // サンプリングレート (例: 44100)
    uint32_t byte_rate;                 // 1秒あたりのバイト数 (sample_rate * num_channels * bits_per_sample / 8)
    uint16_t block_align;               // 1サンプルあたりのバイト数 (num_channels * bits_per_sample / 8)
    uint16_t bits_per_sample = 16;      // 16bit

    // data チャンク (波形データ本体)
    char data_id[4] = {'d', 'a', 't', 'a'};
    uint32_t data_size;                 // 音声データ本体のバイト数
};
#pragma pack(pop)

int main() {
    // 1. 基本パラメータ設定（厳密な整数型で管理）
    const uint32_t sample_rate = 44100;
    const float frequency = 440.0f;     // A4 (ラ) の音
    const float duration = 3.0f;        // 3秒間
    const float volume = 0.5f;          // 音量 (0.0 〜 1.0)
    const std::string filename = "output.wav";

    // 総サンプル数の計算
    const size_t total_samples = static_cast<size_t>(static_cast<double>(sample_rate) * duration);

    // 2. 音声波形データの生成 (16bit整数リニアPCM用のバッファ)
    std::vector<int16_t> audio_data(total_samples);

    for (size_t i = 0; i < total_samples; ++i) {
        // C++23に基づき、誤差を最小化してサイン波を計算
        double time = static_cast<double>(i) / sample_rate;
        double radian = 2.0 * std::numbers::pi * frequency * time;
        
        // -1.0 〜 1.0 の浮動小数点を、16bit整数の範囲（-32768 〜 32767）に変換
        float raw_sample = static_cast<float>(std::sin(radian) * volume);
        audio_data[i] = static_cast<int16_t>(raw_sample * 32767.0f);
    }
    std::cout << "[C++] 3秒間のサイン波（440Hz）をメモリ上に生成しました。\n";

    // 3. WAVヘッダーの組み立て
    WavHeader header;
    header.sample_rate = sample_rate;
    header.data_size = static_cast<uint32_t>(audio_data.size() * sizeof(int16_t));
    header.file_size = header.data_size + 36;
    header.block_align = static_cast<uint16_t>(header.num_channels * (header.bits_per_sample / 8));
    header.byte_rate = header.sample_rate * header.block_align;

    // 4. ファイルへの書き出し（バイナリモード）
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "[エラー] ファイルを作成できませんでした。\n";
        return 1;
    }

    // ヘッダー (44バイト) を書き込む
    ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
    
    // 音声データ本体を書き込む
    ofs.write(reinterpret_cast<const char*>(audio_data.data()), header.data_size);

    std::cout << "[完了] " << filename << " を出力しました (" << header.file_size + 8 << " バイト)。\n";
    
    // vectorのメモリおよびファイルストリームは、スコープを抜けるためC++の標準RAIIにより自動で100%解放されます
    return 0;
}
