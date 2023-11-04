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