#include "daisy_seed.h"
#include "daisysp.h"
#include <stdio.h>
#include "sample.h"
#include "Utility/dsp.h"
#include <cstdlib>


using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed   hw;
Phasor      phs;
Phasor      phsImp;
Phasor      phs2;
Phasor      phsImp2;
float       PI               = 3.14159265358979323846f;
const float sampleSize       = 10312;
float       cosEnv[256]      = {0};
float       potValue         = 1;
float       potValue2        = 1;
float       potValue3        = 1;
float       speed            = 100;
const int   num_adc_channels = 3;
int         printTimer       = 10000;
float       transposition    = 0;
float       sampleFrequency  = 0;
float       grainSize        = 0;
float       transp           = 0;
float       idxTransp        = 0;
float       test             = 0;


uint32_t wrapIdx(uint32_t idx, uint32_t size)
{
    if(idx > size)
    {
        idx = idx - size;
        return idx;
    }

    return idx;
}

float cents2ratio(float cents)
{
    return powf(2.0f, cents / 1200.0f);
}

float ms2samps(float ms, float samplerate)
{
    return (ms * 0.001f) * samplerate;
}

//this function outputs inverted phase values if negative frequency is passed

float negativeInvert(Phasor* phs, float frequency)
{
    return (frequency > 0) ? phs->Process() : ((phs->Process() * -1) + 1);
}

//serial print floats at each

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    for(size_t i = 0; i < size; i++)
    {
        float idxTransp  = (negativeInvert(&phsImp, transp)
                           * ms2samps(grainSize, hw.AudioSampleRate()));
        float idxTransp2 = (negativeInvert(&phsImp2, transp)
                            * ms2samps(grainSize, hw.AudioSampleRate()));


        float    idxSpeed  = (phs.Process() * sampleSize);
        float    idxSpeed2 = (phs2.Process() * sampleSize);
        uint32_t idx  = wrapIdx((uint32_t)(idxSpeed + idxTransp), sampleSize);
        test          = idxSpeed;
        uint32_t idx2 = wrapIdx((uint32_t)(idxSpeed2 + idxTransp2), sampleSize);
        float    sig  = sample[idx] * cosEnv[(uint32_t)(phs.Process() * 256)];
        float    sig2 = sample[idx2] * cosEnv[(uint32_t)(phs2.Process() * 256)];
        out[0][i] = out[1][i] = (sig + sig2) / 2;
    }
}

int main(void)
{
    hw.Configure();
    hw.Init();
    hw.StartLog(true);
    float sample_rate = hw.AudioSampleRate();
    phs.Init(sample_rate, 0, 0);
    phsImp.Init(sample_rate, 0, 0);
    phsImp2.Init(sample_rate, 0, 0.5f);
    phs2.Init(sample_rate, 0, 0);
    AdcChannelConfig my_adc_config[num_adc_channels];
    my_adc_config[0].InitSingle(hw.GetPin(15));
    my_adc_config[1].InitSingle(hw.GetPin(16));
    my_adc_config[2].InitSingle(hw.GetPin(17));
    hw.adc.Init(my_adc_config, num_adc_channels);
    // phs2.Init(sample_rate);
    // phs2.SetFreq(0.5);
    //fills cosEnv with a cosine wave with amplitude from 0 to 1
    for(int i = 0; i < 256; i++)
    {
        cosEnv[i] = sinf((i / 256.0f) * PI);
    }
    hw.StartAudio(AudioCallback);
    hw.adc.Start();
    while(1)
    {
        potValue = hw.adc.GetFloat(0);
        //potValue = 1;
        potValue2       = hw.adc.GetFloat(1);
        potValue3       = hw.adc.GetFloat(2);
        grainSize       = (potValue3 * 100) + 3;
        speed           = (((potValue * 100) / 100) * sampleFrequency);
        sampleFrequency = sample_rate / sampleSize;
        // mapst to -1220 to 1200
        transposition = ((potValue2 * 2400) - 1200);

        transp = ((cents2ratio(transposition) - potValue)) * (1000 / grainSize);
        phs.SetFreq((speed / 2));
        phs2.SetFreq((speed / 2));
        phsImp.SetFreq(abs(transp));
        phsImp2.SetFreq(abs(transp));

        if(printTimer == 10000)
        {
            printTimer = 0;
            hw.PrintLine("Size: " FLT_FMT3, FLT_VAR3(grainSize));
            hw.PrintLine("Transposition: " FLT_FMT3, FLT_VAR3(transposition));
            hw.PrintLine("Speed: " FLT_FMT3, FLT_VAR3(speed));
                        hw.PrintLine("Debug transp: " FLT_FMT3, FLT_VAR3(transp));
            hw.PrintLine("Debug transp: " FLT_FMT3, FLT_VAR3(negativeInvert(&phsImp, transp)));
            // hw.PrintLine( "Scaled pot value: %d",(wrapIdx((uint32_t)(phs.Process() * sampleSize), sampleSize)));
        }
        printTimer++;
    }
}
