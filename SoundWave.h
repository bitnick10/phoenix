#pragma once

#include <cstdint>
#include <stdio.h>
#include <string>
// https://en.wikipedia.org/wiki/Scientific_pitch_notation
std::string GetPianoNoteByFrequency(int frequency, int frequencyA4) {
    //                 0    1     2    3     4    5    6     7    8     9    10   11
    char* notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    double arg = pow(2, 1.0 / 12);
    double frequencyC4 = (double)frequencyA4 / pow(arg, 9);
    auto a = (double)frequency / frequencyC4;
    int step = (int)(log2(pow(a, 12)) + 0.5);
    int index = (step + 12 * 100) % 12;
    return std::string(notes[index]);
}
int GetPianoOctaveByFrequcny(int frequency, int frequencyA4) {
    double arg = pow(2, 1.0 / 12);
    // C0 frequency should be 16.352
    double frequencyC0 = (double)frequencyA4 / pow(arg, 9 + 48);
    auto a = (double)frequency / frequencyC0;
    auto powValue = pow(a, 12);
    // log2Value must be bigger than 0
    auto log2Value = log2(powValue);
    int step = (int)(log2Value + 0.5);
    int octave = step / 12;
    return octave;
}
class SoundWave {
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

    typedef struct WaveStruct {
        FILE *fp;                  //file pointer
        wave_header_t header;      //header
        uint8_t data_flag[4];        //数据标识符
        uint32_t length;             //采样数据总数
        uint32_t *pData;             //data
    } wave_t;
    wave_t wave;
public:
    SoundWave(const char* filename) {
        SoundWave(filename, 0, 0);
    }
    SoundWave(const char* file, int raw, int mono_stereo) {
        unsigned char temp = 0;
        uint8_t read_bytes = 0;
        char *channel_mappings[] = { NULL, "mono", "stereo" };
        uint32_t total_time = 0;
        struct PlayTime {      //播放时间
            uint8_t hour;
            uint8_t minute;
            uint8_t second;
        } play_time;

        if (NULL == (wave.fp = fopen_s(file, "rb"))) {                             /* open file */
            printf("file %s open failure!\n", file);
        }

        /* read heade information */
        if (4 != fread(wave.header.riff, sizeof(uint8_t), 4, wave.fp)) {         /* RIFF chunk */
            printf("read riff error!\n");
            return;
        }
        if (1 != fread(&wave.header.size, sizeof(uint32_t), 1, wave.fp)) {       /* SIZE : from here to file end */
            printf("read size error!\n");
            return;
        }
        if (4 != fread(wave.header.wave_flag, sizeof(uint8_t), 4, wave.fp)) {    /* wave file flag */
            printf("read wave_flag error!\n");
            return;
        }
        if (4 != fread(wave.header.fmt, sizeof(uint8_t), 4, wave.fp)) {           /* fmt chunk */
            printf("read fmt error!\n");
            return;
        }
        if (1 != fread(&wave.header.fmt_len, sizeof(uint32_t), 1, wave.fp)) {     /* fmt length */
            printf("read fmt_len error!\n");
            return;
        }
        if (1 != fread(&wave.header.tag, sizeof(uint16_t), 1, wave.fp)) {         /* tag : PCM or not */
            printf("read tag error!\n");
            return;
        }
        if (1 != fread(&wave.header.channels, sizeof(uint16_t), 1, wave.fp)) {    /* channels */
            printf("read channels error!\n");
            return;
        }
        if (1 != fread(&wave.header.samp_freq, sizeof(uint32_t), 1, wave.fp)) {    /* samp_freq */
            printf("read samp_freq error!\n");
            return;
        }
        if (1 != fread(&wave.header.byte_rate, sizeof(uint32_t), 1, wave.fp)) {    /* byte_rate : decode how many bytes per second */
            /* byte_rate = samp_freq * bit_samp */
            printf("read byte_rate error!\n");
            return;
        }
        if (1 != fread(&wave.header.block_align, sizeof(uint16_t), 1, wave.fp)) {     /* quantize bytes for per samp point */
            printf("read byte_samp error!\n");
            return;
        }
        if (1 != fread(&wave.header.bit_samp, sizeof(uint16_t), 1, wave.fp)) {      /* quantize bits for per samp point */
            /* bit_samp = byte_samp * 8 */
            printf("read bit_samp error!\n");
            return;
        }

        /* jump to "data" for reading data */
        do {
            fread(&temp, sizeof(unsigned char), 1, wave.fp);
        } while ('d' != temp);
        wave.data_flag[0] = temp;
        if (3 != fread(&wave.data_flag[1], sizeof(uint8_t), 3, wave.fp)) {               /* data chunk */
            printf("read header data error!\n");
            return;
        }
        if (1 != fread(&wave.length, sizeof(uint32_t), 1, wave.fp)) {              /* data length */
            printf("read length error!\n");
        }

        /* jduge data chunk flag */
        if (!strcmp((const char*)wave.data_flag, "data")) {
            printf("error : cannot read data!\n");
            return;
        }

        total_time = wave.length / wave.header.byte_rate;
        play_time.hour = (uint8_t)(total_time / 3600);
        play_time.minute = (uint8_t)((total_time / 60) % 60);
        play_time.second = (uint8_t)(total_time % 60);
        /* printf file header information */
        printf("%s %ldHz %dbit, DataLen: %ld, Rate: %ld, Length: %2ld:%2ld:%2ld\n",
               channel_mappings[wave.header.channels],             //声道
               wave.header.samp_freq,                              //采样频率
               wave.header.bit_samp,                               //每个采样点的量化位数
               wave.length,
               wave.header.byte_rate,
               play_time.hour, play_time.minute, play_time.second);


        fclose(wave.fp);                                               /* close wave file */
    }
};