#pragma once

#include <rack.hpp>
using namespace rack;

struct SRV {
  dsp::SchmittTrigger trigger;
  const float stepWidth = 10.F / 128;
  float evenDistOut = 0.F;
  std::mt19937 gen;
  std::uniform_real_distribution<float> uniformDist;

  explicit SRV(std::mt19937 gen_) : gen{gen_} {
    std::uniform_real_distribution<float> uniformDist(0.F, 10.F);
  }

  void process(const float& triggerInput, const float& biasCV, const float& biasParam) {
    float bias = math::clamp(biasParam + biasCV / 10.F, 0.F, 1.F);

    if (trigger.process(triggerInput, 0.1F, 2.F)) {
      float rv = uniformDist(gen);
      while (rv < 0.F || rv > 10.F) {
        rv = uniformDist(gen);
      }
      evenDistOut = quantize(uniformDist(gen), stepWidth);
    }
  }

  float getEvenDistOut() const {
    return evenDistOut;
  }

  // quantize `voltage` to the nearest multiple of `stepSize`
  static float quantize(float voltage, float stepSize) {
    float div = voltage / stepSize;
    float below = std::floorf(div) * stepSize;
    float above = std::ceilf(div) * stepSize;

    float d1 = std::abs(voltage - below);
    float d2 = std::abs(above - voltage);

    float r = NAN;
    if (d1 < d2) {
      r = below;
    } else {
      r = above;
    }
    return r;
  }
};
