#include "plugin.hpp"

struct Random : Module {
  enum ParamId {
    FLUCRATE_PARAM,
    QUANTSTATES_PARAM,
    STOREDPROB_PARAM,
    PARAMS_LEN
  };
  enum InputId {
    FLUCRATECV_INPUT,
    QUANTTRIGGER_INPUT,
    QUANTSTATESCV_INPUT,
    STOREDTRIGGER_INPUT,
    STOREDPROBCV_INPUT,
    INPUTS_LEN
  };
  enum OutputId {
    FLUC_OUTPUT,
    QUANTLIN_OUTPUT,
    QUANTEXP_OUTPUT,
    STOREDEVEN_OUTPUT,
    STOREDDIST_OUTPUT,
    OUTPUTS_LEN
  };
  enum LightId {
    FLUC_LIGHT,
    QUANTLIN_LIGHT,
    QUANTEXP_LIGHT,
    STOREDEVEN_LIGHT,
    STOREDDIST_LIGHT,
    LIGHTS_LEN
  };

  Random() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    configInput(FLUCRATECV_INPUT, "FRV Rate CV");
    configParam(FLUCRATE_PARAM, std::log2(0.05F), std::log2(50.F),
                std::log2(5.F), "Rate", "Hz", 2.F);
    configOutput(FLUC_OUTPUT, "FRV");

    configInput(QUANTTRIGGER_INPUT, "QRV Trigger");
    configInput(QUANTSTATESCV_INPUT, "QRV States CV");
    configParam(QUANTSTATES_PARAM, 1.F, 6.F, 1.F, "States");
    getParamQuantity(QUANTSTATES_PARAM)->snapEnabled = true;
    configOutput(QUANTEXP_OUTPUT, "QRV 2^n");
    configOutput(QUANTLIN_OUTPUT, "QRV n+1");

    configInput(STOREDTRIGGER_INPUT, "SRV Trigger");
    configInput(STOREDPROBCV_INPUT, "SRV Probability CV");
    configParam(STOREDPROB_PARAM, 0.F, 1.F, 0.F, "SRV Probability");
    configOutput(STOREDEVEN_OUTPUT, "SRV Even Dist");
    configOutput(STOREDDIST_OUTPUT, "SRV Variable Dist");

    // these coefficients are lifted from the Fundamental plugin's Noise module
    const float b[] = {0.00425611, 0.00425611};
    const float a[] = {-0.99148778};
    redFilter.setCoefficients(b, a);
  }

  dsp::IIRFilter<2, 2> redFilter;
  float triPhase = 0.F;
  const float triangleFreq = 10.F;  // noisy triangle's frequency
  Clock frvClock;
  dsp::RCFilter flucTriFilter;

  void process(const ProcessArgs& args) override {
    float frvRateCV = inputs[FLUCRATECV_INPUT].getVoltage();
    float frvRateParam = params[FLUCRATE_PARAM].getValue();
    float frvClockFreq = dsp::exp2_taylor5(frvRateParam + (frvRateCV / 2.F));

    float triSample = generateNoisyTriangle(args);
    outputs[QUANTEXP_OUTPUT].setVoltage(triSample);
    if (frvClock.process(args.sampleTime, frvClockFreq)) {
      /* DEBUG("FRV Sample: %f", triSample); */
      flucTriFilter.setCutoffFreq(frvClockFreq * 20 / args.sampleRate);
      flucTriFilter.process(triSample);
    }
    outputs[FLUC_OUTPUT].setVoltage(flucTriFilter.lowpass());
  }

  // generate the "noisy triangle" that is sampled to generate more "musical"
  // random numbers this strategy, like everything else about this module, is
  // copied from the Buchla 266
  //
  // any constants here were arrived at experimentally
  // to get a nice looking wave
  float generateNoisyTriangle(const ProcessArgs& args) {
    float redNoise = redFilter.process(random::normal());
    redNoise *= 25.F;
    float freq = triangleFreq + redNoise * 10;
    advanceTrianglePhase(freq, args.sampleTime);
    float y = 10 * abs((triPhase * 2) - 1);
    return y;
  }

  void advanceTrianglePhase(float freq, float sampleTime) {
    triPhase += freq * sampleTime;
    triPhase -= std::floor(triPhase);
  }
};

struct RandomWidget : ModuleWidget {
  explicit RandomWidget(Random* module) {
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/Random.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(
        createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(
        Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(
        box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(34.29, 21.42)), module, Random::FLUCRATE_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(34.29, 64.26)), module, Random::QUANTSTATES_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(34.29, 107.1)), module, Random::STOREDPROB_PARAM));

    addInput(createInputCentered<BlueBananaPort>(
        mm2px(Vec(11.43, 21.42)), module, Random::FLUCRATECV_INPUT));
    addInput(createInputCentered<RedBananaPort>(
        mm2px(Vec(11.43, 42.84)), module, Random::QUANTTRIGGER_INPUT));
    addInput(createInputCentered<BlueBananaPort>(
        mm2px(Vec(11.43, 64.26)), module, Random::QUANTSTATESCV_INPUT));
    addInput(createInputCentered<RedBananaPort>(
        mm2px(Vec(11.43, 85.68)), module, Random::STOREDTRIGGER_INPUT));
    addInput(createInputCentered<BlueBananaPort>(
        mm2px(Vec(11.43, 107.1)), module, Random::STOREDPROBCV_INPUT));

    addOutput(createOutputCentered<BlueBananaPort>(
        mm2px(Vec(79.897, 21.436)), module, Random::FLUC_OUTPUT));
    addOutput(createOutputCentered<BlueBananaPort>(
        mm2px(Vec(80.01, 42.84)), module, Random::QUANTLIN_OUTPUT));
    addOutput(createOutputCentered<BlueBananaPort>(
        mm2px(Vec(80.01, 64.26)), module, Random::QUANTEXP_OUTPUT));
    addOutput(createOutputCentered<BlueBananaPort>(
        mm2px(Vec(80.01, 85.68)), module, Random::STOREDEVEN_OUTPUT));
    addOutput(createOutputCentered<BlueBananaPort>(
        mm2px(Vec(80.01, 107.1)), module, Random::STOREDDIST_OUTPUT));

    addChild(createLightCentered<SmallLight<GreenRedLight>>(
        mm2px(Vec(84.897, 16.42)), module, Random::FLUC_LIGHT));
    addChild(createLightCentered<SmallLight<GreenRedLight>>(
        mm2px(Vec(85.01, 37.84)), module, Random::QUANTLIN_LIGHT));
    addChild(createLightCentered<SmallLight<GreenRedLight>>(
        mm2px(Vec(85.01, 59.26)), module, Random::QUANTEXP_LIGHT));
    addChild(createLightCentered<SmallLight<GreenRedLight>>(
        mm2px(Vec(85.01, 80.68)), module, Random::STOREDEVEN_LIGHT));
    addChild(createLightCentered<SmallLight<GreenRedLight>>(
        mm2px(Vec(85.01, 102.1)), module, Random::STOREDDIST_LIGHT));
  }
};

Model* modelRandom = createModel<Random, RandomWidget>("Random");
