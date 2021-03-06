#include <gtest/gtest.h>
#include "phoenix/Color.h"
#include "phoenix/SoundWave.h"
TEST(HSB96ToRGB96, SimpleOne) {
    using namespace phoenix;
    HSB96 hsb;
    hsb.h = 0;
    hsb.s = 1.0f;
    hsb.b = 1.0f;
    RGB96 rgb96 = HSB96ToRGB96(hsb);
    EXPECT_EQ(RGB96(1.0f, 0, 0), rgb96);
    hsb.h = 60;
    hsb.s = 1.0f;
    hsb.b = 1.0f;
    rgb96 = HSB96ToRGB96(hsb);
    EXPECT_EQ(RGB96(1.0f, 1.0f, 0), rgb96);
    hsb.h = 25;
    hsb.s = 0.39f;
    hsb.b = 0.57f;
    rgb96 = HSB96ToRGB96(hsb);
    auto rgb24 = RGB96ToRGB24(rgb96);
    EXPECT_EQ(RGB24(145, 112, 89), rgb24);
    hsb.h = 58;
    hsb.s = 0.95f;
    hsb.b = 0.80f;
    rgb96 = HSB96ToRGB96(hsb);
    rgb24 = RGB96ToRGB24(rgb96);
    EXPECT_EQ(RGB24(204, 198, 10), rgb24);
}
TEST(SoundWaveTest, SimpleOne) {
    SoundWave wave("ëë��.wav");
    // uint8_t r[4] = "RIFF";
    EXPECT_EQ('R', wave.header.riff[0]);
    EXPECT_EQ('I', wave.header.riff[1]);
    EXPECT_EQ('F', wave.header.riff[2]);
    EXPECT_EQ('F', wave.header.riff[3]);
    EXPECT_EQ(44100, wave.header.samp_freq);
    EXPECT_EQ(176400, wave.header.byte_rate);
}
int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}