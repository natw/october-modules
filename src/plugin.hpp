#pragma once
#include <rack.hpp>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin *pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;

extern Model *modelSSG;
extern Model *modelRandom;

struct BlackBananaPort : app::SvgPort {
  BlackBananaPort() {
    setSvg(Svg::load(asset::plugin(pluginInstance, "res/jack-black-flat.svg")));
  }
};

struct WhiteBananaPort : app::SvgPort {
  WhiteBananaPort() {
    setSvg(Svg::load(asset::plugin(pluginInstance, "res/jack-white-flat.svg")));
  }
};

struct RedBananaPort : app::SvgPort {
  RedBananaPort() {
    setSvg(Svg::load(asset::plugin(pluginInstance, "res/jack-red-flat.svg")));
  }
};

struct OrangeBananaPort : app::SvgPort {
  OrangeBananaPort() {
    setSvg(
        Svg::load(asset::plugin(pluginInstance, "res/jack-orange-flat.svg")));
  }
};

struct BlueBananaPort : app::SvgPort {
  BlueBananaPort() {
    setSvg(Svg::load(asset::plugin(pluginInstance, "res/jack-blue-flat.svg")));
  }
};

struct NKK2 : app::SvgSwitch {
  NKK2() {
    shadow->opacity = 0.0;
    addFrame(Svg::load(asset::system("res/ComponentLibrary/NKK_0.svg")));
    addFrame(Svg::load(asset::system("res/ComponentLibrary/NKK_2.svg")));
  }
};

struct Clock {
  dsp::Timer timer;
  // takes a time since last sample and frequency in Hz,
  // and returns true if the clock triggers
  bool process(float sampleTime, float freq) {
    timer.process(sampleTime);
    if (timer.getTime() >= 1 / freq) {
      timer.reset();
      return true;
    }
    return false;
  }
};

// this is the noisy triangle that the 266 generates internally to sample from
struct NoisyTriangle {
  dsp::IIRFilter<2, 2> redFilter;
  float phase = 0.F;
  const float triangleFreq = 10.F;  // noisy triangle's frequency

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
