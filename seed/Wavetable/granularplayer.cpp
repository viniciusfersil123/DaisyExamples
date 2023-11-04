#include "granularplayer.h"

using namespace daisysp;

uint32_t GranularPlayer::wrapIdx(uint32_t idx, uint32_t sz)
{
    if(idx > sz)
    {
        idx = idx - sz;
        return idx;
    }

    return idx;
}

float GranularPlayer::cents2ratio(float cents)
{
    return powf(2.0f, cents / 1200.0f);
}


float GranularPlayer::ms2samps(float ms, float samplerate)
{
    return (ms * 0.001f) * samplerate;
}

float GranularPlayer::negativeInvert(Phasor* phs, float frequency)
{
    return (frequency > 0) ? phs->Process() : ((phs->Process() * -1) + 1);
}

float GranularPlayer::Process(float speed, float transposition, float grain_size)
{
    grain_size_    = grain_size;
    speed_         = speed;
    transposition_ = transposition;
    float idxTransp
        = (negativeInvert(&phsImp_, transposition) * ms2samps(grain_size_, sample_rate_));
    float idxTransp2
        = (negativeInvert(&phsImp2_, transposition) * ms2samps(grain_size_, sample_rate_));
    float idxSpeed = negativeInvert(&phs_, speed) * size_;
    float idxSpeed2
        = negativeInvert(&phs2_, speed) * size_;
    uint32_t idx  = wrapIdx((uint32_t)(idxSpeed + idxTransp), size_);
    uint32_t idx2 = wrapIdx(
        (uint32_t)(idxSpeed2 + idxTransp2), size_);
    float sig = sample_[idx]
                * cosEnv_[(uint32_t)(phs_.Process() * 256)];
    float sig2 = sample_[idx2]
                 * cosEnv_[(uint32_t)(phs2_.Process() * 256)];
    return (sig + sig2) / 2;
}