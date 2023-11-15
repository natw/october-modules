#pragma once

#include <rack.hpp>
using namespace rack;

struct SRV {
  const float stepWidth = 10.0 / 128;
  std::mt19937 gen;
  std::uniform_real_distribution<float> dist;

  SRV() {
    std::random_device rd;
    std::mt19937 gen2(rd());
    std::uniform_real_distribution<float> dist(0.F, 10.F);
  }

  void process() {
  }
};
