#pragma once
#include <rack.hpp>

using namespace rack;

struct FRV {
  float previousValue = 0.F;
  float nextValue     = 0.F;
  float phase         = 0.F;

  void process(float sampleTime, float input, Param* rateParam, Port* rateCVInput, Output* output,
               Light* light) {
    float rate      = rateParam->getValue() + (rateCVInput->getVoltage() / 2.F);
    float clockFreq = dsp::exp2_taylor5(rate);

    if (phase >= 1.F) {
      previousValue = nextValue;
      nextValue     = input;
      phase         = 0.F;
    }

    phase += clockFreq * sampleTime;
    float c    = std::cosf(M_PI * phase);
    float fluc = rescale(c, 1.F, -1.F, previousValue, nextValue);
    output->setVoltage(fluc);
    light->setSmoothBrightness(fluc / 10.F, sampleTime);
  }
};
