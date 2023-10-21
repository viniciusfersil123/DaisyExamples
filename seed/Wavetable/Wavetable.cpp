#include "daisy_seed.h"
#include "daisysp.h"
#include <stdio.h>
#include "sample.h"
#include "Utility/dsp.h"


using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;
Phasor    phs;
Phasor    phs2;
float     PI               = 3.14159265358979323846f;
const int sampleSize       = 10463;
float     cosEnv[256]      = {0};
int       chunkSize        = 0;
float     potValue         = 1;
float     potValue2        = 1;
float     potValue3        = 1;
float     speed            = 100;
bool      state            = true;
const int num_adc_channels = 3;
float     ms               = 0;
float     samps            = 0;
float     transposition    = 0;
float     transp           = 0;

uint32_t wrapIdx(uint32_t idx, uint32_t size)
{
    if(idx > size)
    {
        idx = idx - size;
        return idx;
    }

    return idx;
}

float ms2samps(float ms, float samplerate)
{
    return (ms * 0.001f) * samplerate;
}

float cents2ratio(float cents)
{
    return pow(2, cents / 1200);
}

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    for(size_t i = 0; i < size; i++)
    {
        uint32_t idxTransp = (uint32_t)(phs.Process() * samps);
        uint32_t idxSpd    = (uint32_t)(phs2.Process() * sampleSize);
        float    sig = sample[(uint32_t)(wrapIdx((idxSpd + idxTransp), sampleSize))]
                    * cosEnv[(uint32_t)(phs2.Process() * 256)];
        out[0][i] = out[1][i] = sig;
    }
}

int main(void)
{
    //hw.Configure();
    hw.Init();
    float sample_rate = hw.AudioSampleRate();
    phs.Init(sample_rate);
    phs2.Init(sample_rate);
    phs2.SetFreq((sample_rate * potValue / sampleSize));
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
        ms        = pow(2, potValue * 2400);
        // maps 0-1 potValue to -1200 to 1200 cents
        transposition = cents2ratio((potValue2 * 2400) - 1200);
        samps         = ms2samps(ms, sample_rate);
        speed = ((pow(2, potValue * 10)) / 100) * (sample_rate / sampleSize);
        transp
            = (transposition - ((pow(2, potValue * 10)) / 100)) * (1000 / ms);
        phs.SetFreq(transp);
        phs2.SetFreq(speed);
    }
}
