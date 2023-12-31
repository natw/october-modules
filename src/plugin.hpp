#pragma once
#include <rack.hpp>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;

extern Model* modelSSG;
extern Model* modelRandom;
extern Model* modelNCOM;

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
    setSvg(Svg::load(asset::plugin(pluginInstance, "res/jack-orange-flat.svg")));
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
