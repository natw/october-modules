#pragma once
#include <rack.hpp>
using namespace rack;

// this is the noisy triangle that the 266 generates internally to sample from
struct NoisyTriangle {
  dsp::IIRFilter<2, 2> redFilter;
  float                phase        = 0.F;
  const float          triangleFreq = 10.F;  // noisy triangle's frequency

  NoisyTriangle() {
    // these coefficients are lifted from the Fundamental plugin's Noise module
    const float b[] = {0.00425611, 0.00425611};
    const float a[] = {-0.99148778};
    redFilter.setCoefficients(b, a);
  }

  float generate(float sampleTime) {
    float redNoise = redFilter.process(random::normal());
    redNoise *= 25.F;
    float freq = triangleFreq + redNoise * 10;
    advancePhase(freq, sampleTime);
    float y = 10 * abs((phase * 2) - 1);
    return y;
  }

  void advancePhase(float freq, float sampleTime) {
    phase += freq * sampleTime;
    phase -= std::floor(phase);
  }
};
