#pragma once
#ifndef DSY_GRANULARPLAYER_H
#define DSY_GRANULARPLAYER_H
#include <stdint.h>
#include <cmath>
//TODO:Erase and and imports phasor
#include "Control/phasor.h"
#ifdef __cplusplus
using namespace daisysp;

class GranularPlayer
{
  public:
    GranularPlayer() {}
    ~GranularPlayer() {}

    inline void Init(float* sample, int size, float sample_rate)
    {
        sample_      = sample;
        size_        = size;
        sample_rate_ = sample_rate;
        phs_.Init(sample_rate_, 0, 0);
        phsImp_.Init(sample_rate_, 0, 0);
        phs2_.Init(sample_rate_, 0, 0.5f);
        phsImp2_.Init(sample_rate_, 0, 0);
    }

    uint32_t wrapIdx(uint32_t idx, uint32_t size);

    float cents2ratio(float cents);

    float ms2samps(float ms, float samplerate);

    float negativeInvert(Phasor* phs, float frequency);

    //private:
    float* sample_;
    int    size_;
    float  sample_rate_;
    Phasor phs_;
    Phasor phsImp_;
    Phasor phs2_;
    Phasor phsImp2_;
};


#endif
#endif