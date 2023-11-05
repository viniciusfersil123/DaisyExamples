#include "granularplayer.h"
//TODO:Implement sample and hold for preventing unwanted transposition when changing values
using namespace daisysp;

uint32_t GranularPlayerT::wrapIdx(uint32_t idx, uint32_t sz)
{
    if(idx > sz)
    {
        idx = idx - sz;
        return idx;
    }

    return idx;
}

float GranularPlayerT::cents2ratio(float cents)
{
    return powf(2.0f, cents / 1200.0f);
}


float GranularPlayerT::ms2samps(float ms, float samplerate)
{
    return (ms * 0.001f) * samplerate;
}

float GranularPlayerT::negativeInvert(Phasor* phs, float frequency)
{
    return (frequency > 0) ? phs->Process() : ((phs->Process() * -1) + 1);
}

float GranularPlayerT::Process(float speed,
                              float transposition,
                              float grain_size)
{
    grain_size_    = grain_size;
    speed_         = speed * sample_frequency_;
    transposition_ = (cents2ratio(transposition) - speed)
                     * (grain_size >= 1 ? 1000 / grain_size_ : 1);
    //transposition_ = grain_size >= 1 ? 1000 / grain_size_ : 1;
    //transposition_ = transposition;
    phs_.SetFreq(abs((speed_ / 2)));
    phs2_.SetFreq(abs((speed_ / 2)));
    phsImp_.SetFreq(abs(transposition_));
    phsImp2_.SetFreq(abs(transposition_));
    idxTransp_  = (negativeInvert(&phsImp_, transposition_)
                  * ms2samps(grain_size_, sample_rate_));
    idxTransp2_ = (negativeInvert(&phsImp2_, transposition_)
                   * ms2samps(grain_size_, sample_rate_));
    idxSpeed_   = negativeInvert(&phs_, speed) * size_;
    idxSpeed2_  = negativeInvert(&phs2_, speed) * size_;
    idx_        = wrapIdx((uint32_t)(idxSpeed_ + idxTransp_), size_);
    idx2_       = wrapIdx((uint32_t)(idxSpeed2_ + idxTransp2_), size_);
    sig_        = sample_[idx_] * cosEnv_[(uint32_t)(phs_.Process() * 256)];
    sig2_       = sample_[idx2_] * cosEnv_[(uint32_t)(phs2_.Process() * 256)];
    return (sig_ + sig2_) / 2;
}