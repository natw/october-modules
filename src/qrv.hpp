#pragma once
#include <array>
#include <cmath>
#include <rack.hpp>
#include <random>

using namespace rack;

// most of my understanding of the physical model is
// from https://modularsynthesis.com/roman/buchla266/266sou_pcb2.htm
//
// n+1 output is the normal distribution (ie bell curve)
// 2^n output is the uniform distribution

// This retains the original 266's behavior of the random voltages
// being repeating sequences of 64 values.
// The sequences are populated on module initialization.
//
// on trigger, pick next random voltage for each distribution,
// and quantize to scale defined by `n`.
//
// `n` is read from knob and CV every tick, so output values can change even without
// a trigger input.
struct QRV {
  dsp::SchmittTrigger trigger;
  std::mt19937 gen;
  float nPlusOneOut = 1.0;
  float twoNOut = 1.0;
  int i = 0;
  std::array<float, 64> uniformSeq = {};
  std::array<float, 64> normalSeq = {};

  explicit QRV(std::mt19937 gen_) : gen{gen_} {
    populateRandom();
  }

  void populateRandom() {
    std::uniform_real_distribution<float> uniformDist(0.F, 10.F);
    std::normal_distribution<float> normalDist(5.F, 2.F);

    for (auto& step : uniformSeq) {
      step = uniformDist(gen);
    }

    for (auto& step : normalSeq) {
      step = normalDist(gen);
      while (step < 0.F || step > 10.F) {
        step = normalDist(gen);
      }
    }
  }

  void process(const float& triggerInput, const float& statesCV, const float& statesParam) {
    float n = std::round(math::clamp(statesCV + statesParam, 1.F, 6.F));
    float twoN = std::exp2f(n);

    if (trigger.process(triggerInput, 0.1F, 2.F)) {
      i++;
      i = i % 64;
    }

    nPlusOneOut = quantize(normalSeq[i], 10.F / n);
    twoNOut = quantize(uniformSeq[i], 10.F / (twoN - 1.F));
  }

  float getNPlusOne() const {
    return nPlusOneOut;
  }

  float getTwoNOut() const {
    return twoNOut;
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
