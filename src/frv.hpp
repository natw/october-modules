#pragma once
#include <rack.hpp>

using namespace rack;

struct FRV {
  float previousValue = 0.F;
  float nextValue = 0.F;
  float phase = 0.F;
  float output = 0.F;

  void process(const float& sampleTime, float& input, const float& rateParam, const float& rateCV) {
    float rate = rateParam + (rateCV / 2.F);
    float clockFreq = dsp::exp2_taylor5(rate);

    if (phase >= 1.F) {
      previousValue = nextValue;
      nextValue = input;
      phase = 0.F;
    }

    phase += clockFreq * sampleTime;
    float c = std::cos(M_PI * phase);
    float fluc = rescale(c, 1.F, -1.F, previousValue, nextValue);

    output = fluc;
  }

  float getOutput() const {
    return output;
  }
};
