#pragma once
#include <math.h>

#include <rack.hpp>
#include <random>

using namespace rack;

// from https://modularsynthesis.com/roman/buchla266/266sou_pcb2.htm
//  n   |  2^n step size  |  n+1 step size
// ---------------------------------------
//  1   |  10V            |  10V
//  2   |  3.333V         |  5V
//  3   |  1.429V         |  3.333V
//  4   |  667mV          |  2.5V
//  5   |  323mV          |  2V
//  6   |  159mV          |  1.667V
// n+1 output is the normal distribution (ie bell curve)
// 2^n output is the uniform distribution

// on trigger, pick random voltage for each distribution,
// quantize to scale defined by `n`
// on `n` change, requantize
struct QRV {
  dsp::SchmittTrigger trigger;
  int lastN{-1};
  std::random_device rd;
  std::mt19937 gen;
  std::normal_distribution<float> nPlusOneDist;
  float nPlusOneVal = 1.0;
  float nPlusOneOut = 1.0;
  std::uniform_real_distribution<float> twoNDist;
  float twoNVal = 1.0;
  float twoNOut = 1.0;

  QRV() {
    std::mt19937 gen(rd());
  }

  void process(const float &triggerInput, const float &statesCV, const float &statesParam) {
    int n = static_cast<int>(std::round(math::clamp(statesCV + statesParam, 1.F, 6.F)));
    int twoN = 1 << n;

    if (trigger.process(triggerInput, 0.1F, 2.F)) {
      std::normal_distribution<float> nPlusOneDist(5.F, 2.F);
      std::uniform_real_distribution<float> twoNDist(0.1F, 10.F);
      // normal distribution doesn't have a min and max,
      // just a mean and deviation, so if we get something out of bounds,
      // just try again
      nPlusOneVal = nPlusOneDist(gen);
      while (nPlusOneVal < 0.F || nPlusOneVal > 10.F) {
        nPlusOneVal = nPlusOneDist(gen);
      }
      twoNVal = twoNDist(gen);
    }

    nPlusOneOut = quantize(nPlusOneVal, 10.0 / n);
    twoNOut = quantize(twoNVal, 10.0 / (twoN - 1));
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
