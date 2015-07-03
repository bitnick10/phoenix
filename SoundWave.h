#pragma once

#include <cstdint>
#include <stdio.h>
#include <string>
#include <fstream>

inline int GetStepFromC0(int frequency, int frequencyA4) {
    double arg = pow(2, 1.0 / 12);
    double frequencyC0 = (double)frequencyA4 / pow(arg, 9 + 48);
    // C0 frequency should be 16.352
    assert(16 == (int)frequencyC0);
    auto a = (double)frequency / frequencyC0;
    auto powValue = pow(a, 12);
    auto log2Value = log2(powValue);
    assert(log2Value >= 0);
    int step = (int)(log2Value + 0.5);
    return step;
}

// https://en.wikipedia.org/wiki/Scientific_pitch_notation
inline std::string GetPianoNoteByFrequency(int frequency, int frequencyA4) {
    //                 0    1     2    3     4    5    6     7    8     9    10   11
    char* notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int step = GetStepFromC0(frequency, frequencyA4);
    int index = (step + 12 * 100) % 12;
    return std::string(notes[index]);
}

// https://en.wikipedia.org/wiki/Solf%C3%A8ge
inline std::string GetPianoSolfaNumberByFrequency(int frequency, int frequencyA4) {
    char* snnotes[] = { "1", "1#", "2", "2#", "3", "4", "4#", "5", "5#", "6", "6#", "7" };
    int step = GetStepFromC0(frequency, frequencyA4);
    int index = (step + 12 * 100) % 12;
    return std::string(snnotes[index]);
}

inline int GetPianoOctaveByFrequcny(int frequency, int frequencyA4) {
    int step = GetStepFromC0(frequency, frequencyA4);
    int octave = step / 12;
    return octave;
}

class PianoNote {
public:
    int octave;
    int toneIndex;
    auto __Str__() const -> std::string {
        const char* notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        return notes[toneIndex] + std::to_string(octave);
    }
    void SemitoneAdd(int value) {
        assert(toneIndex < 12);
        toneIndex += value;
        if(toneIndex >= 12) {
            octave += toneIndex / 12;
            toneIndex = toneIndex % 12;
        }
    }
    /*void __Str__(const std::string& value) {
        note = value;
    }*/
};

// http://soundfile.sapp.org/doc/WaveFormat/
class SoundWave {
public:
    typedef struct WaveHeader {
        uint8_t riff[4];             //资源交换文件标志
        uint32_t size;               //从下个地址开始到文件结尾的字节数
        uint8_t wave_flag[4];        //wave文件标识
        uint8_t fmt[4];              //波形格式标识
        uint32_t fmt_len;            //过滤字节(一般为00000010H)
        uint16_t tag;                //格式种类，值为1时，表示PCM线性编码
        uint16_t channels;           //通道数，单声道为1，双声道为2
        uint32_t samp_freq;          //采样频率
        uint32_t byte_rate;          //数据传输率 (每秒字节＝采样频率×每个样本字节数)
        uint16_t block_align;        //块对齐字节数 = channles * bit_samp / 8
        uint16_t bit_samp;           //bits per sample (又称量化位数)
    } wave_header_t;

    uint8_t data_flag[4];        //数据标识符
    uint32_t data_length;
    void* data;
    wave_header_t header;

public:
    SoundWave(const char* filename) {
        std::fstream file;
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (file.fail()) {
            return;
        }
        file.read((char*)&header, sizeof(header));
        file.read((char*)data_flag, sizeof(data_flag));
        file.read((char*)&data_length, sizeof(data_length));
        data = malloc(data_length);
        file.read((char*)data, data_length);
        file.close();
        struct PlayTime {      //播放时间
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
        } play_time;
        auto total_time = data_length / header.byte_rate;
        play_time.hour = (uint8_t)(total_time / 3600);
        play_time.minute = (uint8_t)((total_time / 60) % 60);
        play_time.second = (uint8_t)(total_time % 60);
    }
};