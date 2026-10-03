#include "audio_lib.hpp"

int main() {
    const uint32_t sample_rate = 44100;
    SoundGenerator generator(sample_rate);

    std::cout << "--- 実験1: 波形生成 ➡ WAVバッファ化 ➡ ファイル保存 ➡ 明示的解放 ---\n";
    {
        SoundBuffer raw_wav = generator.create_sine(440.0f, 1.5f, 0.5f);
        WavFileBuffer wav_file_buf(raw_wav, sample_rate);
        raw_wav.free_memory();

        WavFileWriter::save("output_mem.wav", wav_file_buf);
        wav_file_buf.free_memory();
    } 

    std::cout << "\n--- 実験2: 解放処理を忘れてもすべてRAIIで安全に自動連鎖解放 ---\n";
    {
        SoundBuffer raw_wav2 = generator.create_sine(523.25f, 1.0f, 0.5f);
        WavFileBuffer wav_file_buf2(raw_wav2, sample_rate);
        WavFileWriter::save("output_raii.wav", wav_file_buf2);
        
        std::cout << "★明示的解放を何も呼ばずにスコープを抜けます...\n";
    } 

    return 0;
}
