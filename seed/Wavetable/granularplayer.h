#pragma once
#ifndef DSY_GRANULARPLAYER_H
#define DSY_GRANULARPLAYER_H
#include <stdint.h>
#include <cmath>
//TODO:Erase and and imports phasor
//TODO:Include header in daisyp.h
#include "Control/phasor.h"
#ifdef __cplusplus
using namespace daisysp;


class GranularPlayer
{
  public:
    GranularPlayer() {}
    ~GranularPlayer() {}
    //TODO:Create overloads for Init
    inline void Init(float* sample, int size, float sample_rate)
    {
        sample_      = sample;
        size_        = size;
        sample_rate_ = sample_rate;
        phs_.Init(sample_rate_, 0, 0);
        phsImp_.Init(sample_rate_, 0, 0);
        phs2_.Init(sample_rate_, 0, 0.5f);
        phsImp2_.Init(sample_rate_, 0, 0);
        for(int i = 0; i < 256; i++)
        {
            cosEnv_[i] = sinf((i / 256.0f) * PI);
        }
    }
    //public:
    float Process(float speed, float transposition, float grain_size);

    //private:
    uint32_t wrapIdx(uint32_t idx, uint32_t size);

    float cents2ratio(float cents);

    float ms2samps(float ms, float samplerate);

    float negativeInvert(Phasor* phs, float frequency);

    //private:
    float* sample_;
    float  sample_rate_;
    int    size_;
    float  grain_size_;    //in ms
    float  speed_;         //in % of original speed
    float  transposition_; //in cents
    float  cosEnv_[256] = {0};
    //removes PI and use PI in utils.h
    float PI = 3.14159265358979323846f;

    Phasor phs_;
    Phasor phsImp_;
    Phasor phs2_;
    Phasor phsImp2_;
};


#endif
#endif