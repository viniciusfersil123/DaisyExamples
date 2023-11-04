#include "daisy_seed.h"
#include "daisysp.h"
#include <stdio.h>
#include "sample.h"
#include "Utility/dsp.h"
#include <cstdlib>
#include "granularplayer.h"


using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed      hw;
float          PI               = 3.14159265358979323846f;
const float    sampleSize       = 10312;
float          cosEnv[256]      = {0};
float          potValue         = 1;
float          potValue2        = 1;
float          potValue3        = 1;
float          speed            = 100;
const int      num_adc_channels = 3;
int            printTimer       = 10000;
float          transposition    = 0;
float          sampleFrequency  = 0;
float          grainSize        = 0;
float          transp           = 0;
float          idxTransp        = 0;
float          test             = 0;
GranularPlayer granularPlayer;

//serial print floats at each

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    for(size_t i = 0; i < size; i++)
    {
        float idxTransp
            = (granularPlayer.negativeInvert(&granularPlayer.phsImp_, transp)
               * granularPlayer.ms2samps(grainSize, hw.AudioSampleRate()));
        float idxTransp2
            = (granularPlayer.negativeInvert(&granularPlayer.phsImp2_, transp)
               * granularPlayer.ms2samps(grainSize, hw.AudioSampleRate()));


        float idxSpeed
            = granularPlayer.negativeInvert(&granularPlayer.phs_, speed)
              * sampleSize;
        float idxSpeed2
            = granularPlayer.negativeInvert(&granularPlayer.phs2_, speed)
              * sampleSize;
        uint32_t idx  = granularPlayer.wrapIdx((uint32_t)(idxSpeed + idxTransp),
                                              sampleSize);
        test          = idxSpeed;
        uint32_t idx2 = granularPlayer.wrapIdx(
            (uint32_t)(idxSpeed2 + idxTransp2), sampleSize);
        float sig = granularPlayer.sample_[idx]
                    * cosEnv[(uint32_t)(granularPlayer.phs_.Process() * 256)];
        float sig2 = granularPlayer.sample_[idx2]
                     * cosEnv[(uint32_t)(granularPlayer.phs2_.Process() * 256)];
        out[0][i] = out[1][i] = (sig + sig2) / 2;
    }
}

int main(void)
{
    /////////////////////////////////////////////////
    hw.Configure();
    hw.Init();
    hw.StartLog(true);
    float            sample_rate = hw.AudioSampleRate();
    AdcChannelConfig my_adc_config[num_adc_channels];
    my_adc_config[0].InitSingle(hw.GetPin(15));
    my_adc_config[1].InitSingle(hw.GetPin(16));
    my_adc_config[2].InitSingle(hw.GetPin(17));
    hw.adc.Init(my_adc_config, num_adc_channels);
    /////////////////////////////////////////////////
    granularPlayer.Init(sample, 10312, sample_rate);
    /////////////////////////////////////////////////

    for(int i = 0; i < 256; i++)
    {
        cosEnv[i] = sinf((i / 256.0f) * PI);
    }
    hw.StartAudio(AudioCallback);
    hw.adc.Start();
    while(1)
    {
        /////////////////////////////////////////////////
        potValue        = (hw.adc.GetFloat(0) * 2) - 1;
        potValue2       = hw.adc.GetFloat(1);
        potValue3       = hw.adc.GetFloat(2);
        grainSize       = (potValue3 * 100) + 3;
        speed           = (((potValue * 100) / 100) * sampleFrequency);
        sampleFrequency = sample_rate / sampleSize;

        transposition = ((potValue2 * 2400) - 1200);

        transp = ((granularPlayer.cents2ratio(transposition) - potValue)) * (1000 / grainSize);
        granularPlayer.phs_.SetFreq(abs((speed / 2)));
        granularPlayer.phs2_.SetFreq(abs((speed / 2)));
        granularPlayer.phsImp_.SetFreq(abs(transp));
        granularPlayer.phsImp2_.SetFreq(abs(transp));

        if(printTimer == 10000)
        {
            printTimer = 0;
            // hw.PrintLine("Size: " FLT_FMT3, FLT_VAR3(grainSize));
            // hw.PrintLine("Transposition: " FLT_FMT3, FLT_VAR3(transposition));
            // hw.PrintLine("Speed: " FLT_FMT3, FLT_VAR3(speed));
            // hw.PrintLine("Debug transp: " FLT_FMT3, FLT_VAR3(transp));
            //  hw.PrintLine( "Scaled pot value: %d", (int)(&sample));
            //  hw.PrintLine( "Scaled pot value: %d", &granularPlayer.sample_);
        }
        printTimer++;
    }
}
