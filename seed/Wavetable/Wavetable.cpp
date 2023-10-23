#include "daisy_seed.h"
#include "daisysp.h"
#include <stdio.h>
#include "sample.h"
#include "Utility/dsp.h"


using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed   hw;
Phasor      phs;
Phasor      phsImp;
Phasor      phs2;
Phasor      phsImp2;
float       PI               = 3.14159265358979323846f;
const float sampleSize       = 10463;
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

//serial print floats at each

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    for(size_t i = 0; i < size; i++)
    {
        float idxTransp
            = (phsImp.Process() * ms2samps(grainSize, hw.AudioSampleRate()));
        float idxTransp2
            = (phsImp2.Process() * ms2samps(grainSize, hw.AudioSampleRate()));
        float    idxSpeed  = (phs.Process() * sampleSize);
        float    idxSpeed2 = (phs2.Process() * sampleSize);
        uint32_t idx  = wrapIdx((uint32_t)(idxSpeed + idxTransp), sampleSize);
        uint32_t idx2 = wrapIdx((uint32_t)(idxSpeed2 + idxTransp2), sampleSize);
        float    sig  = sample[idx] * cosEnv[(uint32_t)(phs.Process() * 256)];
        float    sig2 = sample[idx2] * cosEnv[(uint32_t)(phs2.Process() * 256)];
        out[0][i] = out[1][i] = (sig + sig2)/2;
    }
}

int main(void)
{
    hw.Configure();
    hw.Init();
    hw.StartLog(true);
    float sample_rate = hw.AudioSampleRate();
    phs.Init(sample_rate,0,0);
    phsImp.Init(sample_rate,0,0);
    phsImp2.Init(sample_rate,0,0.5f);
    phs2.Init(sample_rate,0,0);
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
        potValue  = hw.adc.GetFloat(0);
        potValue2 = hw.adc.GetFloat(1);
        potValue3 = hw.adc.GetFloat(2);
        //chunkSize = potValue2 * sampleSize;
        //maps the pot value to the frequency of the phasor exponentially
        grainSize = (potValue3 * 100) + 3;
        speed     = ((potValue * 100) / 100);
        //maps the potvalue2 to tranpostion from -1200 to 1200 cents
        sampleFrequency = sample_rate / sampleSize;
        transposition   = cents2ratio((potValue2 * 2400));
        phs.SetFreq((speed * sampleFrequency) / 2.2);
        transp = (transposition - speed) * (1000 / grainSize);
        phsImp.SetFreq(transp);

        if(printTimer == 10000)
        {
            printTimer = 0;
            hw.PrintLine("Debug: " FLT_FMT3, FLT_VAR3((phsImp.Process())));
            // hw.PrintLine( "Scaled pot value: %d",(wrapIdx((uint32_t)(phs.Process() * sampleSize), sampleSize)));
        }
        printTimer++;
    }
}
