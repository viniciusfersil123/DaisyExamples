#include "daisy_seed.h"
#include "daisysp.h"
#include <stdio.h>
#include "sample.h"
#include "Utility/dsp.h"
#include <cstdlib>
using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;

float potValue;
float potValue2;
float potValue3;

const int num_adc_channels = 3;

int printTimer = 10000;

float speed;
float transposition;
float grainSize;

GranularPlayer granularPlayer;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    for(size_t i = 0; i < size; i++)
    {
        float sig = granularPlayer.Process(speed, transposition, grainSize);
        out[0][i] = out[1][i] = sig;
    }
}

int main(void)
{
    AdcChannelConfig my_adc_config[num_adc_channels];

    hw.Configure();
    hw.Init();

    float sample_rate = hw.AudioSampleRate();

    my_adc_config[0].InitSingle(hw.GetPin(15));
    my_adc_config[1].InitSingle(hw.GetPin(16));
    my_adc_config[2].InitSingle(hw.GetPin(17));

    hw.adc.Init(my_adc_config, num_adc_channels);
    hw.StartAudio(AudioCallback);
    hw.adc.Start();

    size_t sampleSize = sizeof(sample) / sizeof(sample[0]);
    granularPlayer.Init(sample, sampleSize, sample_rate);

    while(1)
    {
        potValue  = hw.adc.GetFloat(0);
        potValue2 = hw.adc.GetFloat(1);
        potValue3 = hw.adc.GetFloat(2);

        //

        grainSize     = (potValue3 * 100) + 3;
        speed         = (potValue * 2) - 1;
        transposition = ((potValue2 * 4800) - 2400);
    }
}
