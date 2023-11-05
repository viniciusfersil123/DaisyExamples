#include "daisy_seed.h"
#include "daisysp.h"
#include <stdio.h>
#include "sample.h"
#include "Utility/dsp.h"
#include <cstdlib>
// #include "granularplayer.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;
int       printTimer = 10000;
DaisySeed hw;

float potValue;
float potValue2;
float potValue3;

const int      num_adc_channels = 3;
float          speed;
float          transposition;
float          grainSize;
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
    hw.StartLog(true);
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
        transposition = ((potValue2 * 2400) - 1200);

        if(printTimer == 10000)
        {
            printTimer = 0;
            // hw.PrintLine("Size: " FLT_FMT3, FLT_VAR3(grainSize));
            //hw.PrintLine("Test: " FLT_FMT3, FLT_VAR3(test));
            hw.PrintLine("Class Speed: " FLT_FMT3, FLT_VAR3(granularPlayer.speed_));
            // hw.PrintLine("Granular Player Transp: " FLT_FMT3,
            //              FLT_VAR3(granularPlayer.transposition_));
            // hw.PrintLine("Debug transp: " FLT_FMT3, FLT_VAR3(transp));

            //  hw.PrintLine( "Scaled pot value: %d", (int)(&sample));
            //  hw.PrintLine( "Scaled pot value: %d", &granularPlayer.sample_);
        }
        printTimer++;
    }
}
