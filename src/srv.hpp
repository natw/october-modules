#pragma once

#include <rack.hpp>
using namespace rack;

struct SRV {
  dsp::SchmittTrigger trigger;
  const float stepWidth = 10.F / 127;
  float uniformOut = 0.F;
  float biasedOut = 0.F;
  std::mt19937 gen;
  std::uniform_real_distribution<float> uniformDist;

  explicit SRV(std::mt19937 gen_) : gen{gen_} {
  }

  void process(const float& triggerInput, const float& biasCV, const float& biasParam) {
    float bias = math::clamp(biasParam + biasCV / 10.F, 0.F, 1.F);

    if (trigger.process(triggerInput, 0.1F, 2.F)) {
      std::uniform_real_distribution<float> uniformDist(0.F, 10.F);
      float rv = uniformDist(gen);
      while (rv < 0.F || rv > 10.F) {
        rv = uniformDist(gen);
      }
      uniformOut = quantize(rv, stepWidth);

      std::normal_distribution<float> biasedDist(bias * 10.F, 2.F);
      biasedOut = biasedDist(gen);
      while (biasedOut < 0.F || biasedOut > 10.F) {
        biasedOut = biasedDist(gen);
      }
      biasedOut = quantize(biasedOut, stepWidth);
    }
  }

  float getUniformOut() const {
    return uniformOut;
  }

  float getBiasedOut() const {
    return biasedOut;
  }

  // quantize `voltage` to the nearest multiple of `stepSize`
  static float quantize(float voltage, float stepSize) {
    float div = voltage / stepSize;
    float below = std::floor(div) * stepSize;
    float above = std::ceil(div) * stepSize;

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
