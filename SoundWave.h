#pragma once

#include <cstdint>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <assert.h>

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
struct Pitch {
    // for piano A#4=1 G#=-1 A5+12 depends on interval in one octave
    // A4 can be 440Hz 442Hz whatever you want this is just steps
    enum class Piano {
        C0 = -57,
        C1 = -45,
        C2 = -33,
        C3 = -21,
        C4 = -9,
        A4 = 0
    };
    int delta;
    Pitch() : delta(0) {
    }
    Pitch(const Pitch& pitch) {
        *this = pitch;
    }
    void operator++() {
        ++delta;
    }
    void operator++(int) {
        delta++;
    }
    void operator+=(int value) {
        delta += value;
    }
    void operator-=(int value) {
        delta -= value;
    }
    Pitch operator+(int value) {
        Pitch ret = *this;
        ret += value;
        return ret;
    }
    Pitch operator-(int value) {
        Pitch ret = *this;
        ret -= value;
        return ret;
    }
    Pitch& operator=(Pitch::Piano pp) {
        this->delta = (int)pp;
        return *this;
    }
    bool operator==(const Pitch& rhs) {
        return this->delta == rhs.delta;
    }
    void operator=(const Pitch& rhs) {
        this->delta = rhs.delta;
    }
    int GetPianoOctave() {
        int C0 = 0 - 9 - 4 * 12;
        return (delta - C0) / 12;
    }
    std::string GetPianoNote() {
        const char* notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        int C4 = -9;
        // 1200 should be enough
        int index = (delta - C4 + 1200) % 12;
        return std::string(notes[index]);
    }
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
        uint16_t block_align;        //块对齐字节数 = channles * bits_per_sample / 8
        uint16_t bits_per_sample;    //bits per sample (又称量化位数)
        uint8_t data_flag[4];        //数据标识符
        uint32_t data_length;
    } wave_header_t;
    wave_header_t header;
    char* data;
public:
    auto __SampleCount__() const -> uint32_t {
        return header.data_length / (header.bits_per_sample / 8);
    }
    SoundWave() {
    }
    SoundWave(uint32_t dataLength) {
        this->header.data_length = dataLength;
        this->data = new char[dataLength];
    }
    SoundWave(const SoundWave& wave) {
        this->header = wave.header;
        this->data = new char[header.data_length];
        memcpy(this->data, wave.data, wave.header.data_length);
    }
    SoundWave(const char* filename) {
        std::fstream file;
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (file.fail()) {
            return;
        }
        file.read((char*)&header, sizeof(header));
        data = new char[header.data_length];
        file.read((char*)data, header.data_length);
        file.close();
        struct PlayTime {      //播放时间
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
        } play_time;
        auto total_time = header.data_length / header.byte_rate;
        play_time.hour = (uint8_t)(total_time / 3600);
        play_time.minute = (uint8_t)((total_time / 60) % 60);
        play_time.second = (uint8_t)(total_time % 60);
    }

    void Save(const char* filename) {
        std::ofstream file;
        file.open(filename, std::ofstream::out | std::ofstream::binary);
        file.write((char*)&header, sizeof(header));
        file.write((char*)data, header.data_length);
        file.close();
    }

    std::vector<SoundWave> SplitIntoEqualPieces(int n) {
        std::vector<SoundWave> ret;
        for (int i = 0; i < n; i++) {
            SoundWave wave(this->header.data_length / n);
            wave.header = this->header;
            wave.header.data_length = this->header.data_length / n;
            wave.header.size = wave.header.data_length + 36;
            assert(wave.header.channels == 1 || wave.header.channels == 2);
            uint32_t offset = i * wave.header.data_length & ~3;
            memcpy(wave.data, this->data + offset , wave.header.data_length);
            ret.push_back(wave);
        }
        return ret;
    }
    ~SoundWave() {
        delete data;
        data = nullptr;
    }
};