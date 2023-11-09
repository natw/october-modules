#include "plugin.hpp"

using dsp::SlewLimiter;

struct SSG : Module {
  enum ParamId {
    SMOOTHRATEVC_PARAM,
    STEPPEDRATEVC_PARAM,
    SMOOTHRATE_PARAM,
    SMOOTHRATEHILO_PARAM,
    STEPPEDRATE_PARAM,
    STEPPEDRATEHILO_PARAM,
    PARAMS_LEN
  };
  enum InputId {
    SMOOTH_INPUT,
    STEPPED_INPUT,
    SMOOTHRATEVC_INPUT,
    STEPPEDRATEVC_INPUT,
    SMOOTHHOLD_INPUT,
    STEPPEDSAMPLE_INPUT,
    INPUTS_LEN
  };
  enum OutputId {
    SMOOTH_OUTPUT,
    STEPPED_OUTPUT,
    SMOOTHCYCLE_OUTPUT,
    STEPPEDCYCLE_OUTPUT,
    COUPLER_OUTPUT,
    COUPLERHOT_OUTPUT,
    OUTPUTS_LEN
  };

  enum LightId { ENUMS(SMOOTH_LIGHT, 2), ENUMS(STEPPED_LIGHT, 2), LIGHTS_LEN };

  enum SmoothRateSwitchStates {
    SMOOTH_LO,
    SMOOTH_HI,
  };

  enum SteppedRateSwitchStates {
    STEPPED_LO,
    STEPPED_HI,
  };

  SSG() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    configParam(SMOOTHRATEVC_PARAM, 0.F, 1.F, 0.5F, "Smooth Rate VC Attenuator",
                "%", 0.F, 100.F);
    configParam(STEPPEDRATEVC_PARAM, 0.F, 1.F, 0.5F,
                "Stepped Rate VC Attenuator", "%", 0.F, 100.F);
    configParam(SMOOTHRATE_PARAM, 0.F, 1000.F, 0.F, "Smooth Rate", "", 0.F,
                .001F);
    configParam(STEPPEDRATE_PARAM, 0.F, 1000.F, 0.F, "Stepped Rate", "", 0.F,
                .001F);
    configSwitch(SMOOTHRATEHILO_PARAM, SMOOTH_LO, SMOOTH_HI, SMOOTH_HI, "Range",
                 {"Low", "High"});
    configSwitch(STEPPEDRATEHILO_PARAM, STEPPED_LO, STEPPED_HI, STEPPED_HI,
                 "Range", {"Low", "High"});

    configInput(SMOOTH_INPUT, "Smooth Input");
    configInput(STEPPED_INPUT, "Stepped Input");
    configInput(SMOOTHRATEVC_INPUT, "Smooth Rate VC");
    configInput(STEPPEDRATEVC_INPUT, "Stepped Rate VC");
    configInput(SMOOTHHOLD_INPUT, "Smooth Hold (Gate) ");
    configInput(STEPPEDSAMPLE_INPUT, "Stepped Sample (Trigger)");

    configOutput(SMOOTH_OUTPUT, "Smooth Output");
    configOutput(STEPPED_OUTPUT, "Stepped Output");
    configOutput(SMOOTHCYCLE_OUTPUT, "Smooth Cycle Output");
    configOutput(STEPPEDCYCLE_OUTPUT, "Stepped Cycle Output");
    configOutput(COUPLER_OUTPUT, "Coupler (0V / 5V)");
    configOutput(COUPLERHOT_OUTPUT, "Coupler (-10V / 10V)");
  }

  float previousSmooth{};
  float previousStepped{};
  SlewLimiter limiter;
  dsp::SchmittTrigger steppedTrigger;
  dsp::SchmittTrigger smoothCycleTrigger;
  dsp::SchmittTrigger steppedCycleTrigger;

  void process(const ProcessArgs &args) override {
    processSmooth(args);
    processStepped(args);
    updateCoupler(args);
  }

  void processSmooth(const ProcessArgs &args) {
    float slewParam = params[SMOOTHRATE_PARAM].getValue();
    float slewCV = inputs[SMOOTHRATEVC_INPUT].getVoltage();
    float slewAttenFactor = params[SMOOTHRATEVC_PARAM].getValue();
    float slewExtra = slewCV * slewAttenFactor * 200.F;

    float slew = (slewParam + slewExtra) * args.sampleTime * 0.1;
    if (params[SMOOTHRATEHILO_PARAM].getValue() == SMOOTH_HI) {
      slew *= 100.F;
    }

    float smoothInput = inputs[SMOOTH_INPUT].getVoltage();
    float newSmooth =
        previousSmooth + math::clamp(smoothInput - previousSmooth, -slew, slew);

    previousSmooth = newSmooth;

    float holdGate = inputs[SMOOTHHOLD_INPUT].getVoltage();
    if (holdGate <= 1.F) {
      outputs[SMOOTH_OUTPUT].setVoltage(newSmooth);
      float b = newSmooth / 10.F;
      lights[SMOOTH_LIGHT].setSmoothBrightness(b, args.sampleTime);
      lights[SMOOTH_LIGHT + 1].setSmoothBrightness(-b, args.sampleTime);
      smoothCycleTrigger.process(newSmooth, 1.F, 4.8F);
      if (smoothCycleTrigger.isHigh()) {
        outputs[SMOOTHCYCLE_OUTPUT].setVoltage(0.F);
      } else {
        outputs[SMOOTHCYCLE_OUTPUT].setVoltage(10.F);
      }
    }
  }

  void processStepped(const ProcessArgs &args) {
    bool steppedTriggered = steppedTrigger.process(
        inputs[STEPPEDSAMPLE_INPUT].getVoltage(), 0.1F, 2.F);

    float slewParam = params[STEPPEDRATE_PARAM].getValue();
    float slewCV = inputs[STEPPEDRATEVC_INPUT].getVoltage();
    float slewAttenFactor = params[STEPPEDRATEVC_PARAM].getValue();
    float slewAdd = slewCV * slewAttenFactor;
    float slew = (slewParam + slewAdd) * args.sampleTime * 0.05F;
    if (params[STEPPEDRATEHILO_PARAM].getValue() == STEPPED_HI) {
      slew *= 10;
    }

    float steppedInput = inputs[STEPPED_INPUT].getVoltage();
    float newStepped = previousStepped +
                       math::clamp(steppedInput - previousStepped, -slew, slew);

    previousStepped = newStepped;
    if (steppedTriggered) {
      outputs[STEPPED_OUTPUT].setVoltage(newStepped);
      float b = newStepped / 10.F;
      lights[STEPPED_LIGHT].setSmoothBrightness(b, args.sampleTime);
      lights[STEPPED_LIGHT + 1].setSmoothBrightness(-b, args.sampleTime);

      steppedCycleTrigger.process(newStepped, 1.F, 4.8F);
      if (steppedCycleTrigger.isHigh()) {
        outputs[STEPPEDCYCLE_OUTPUT].setVoltage(0.F);
      } else {
        outputs[STEPPEDCYCLE_OUTPUT].setVoltage(10.F);
      }
    }
  }

  void updateCoupler(const ProcessArgs & /*args*/) {
    float smooth = outputs[SMOOTH_OUTPUT].getVoltage();
    float stepped = outputs[STEPPED_OUTPUT].getVoltage();

    if (stepped > smooth) {
      outputs[COUPLER_OUTPUT].setVoltage(5.F);
      outputs[COUPLERHOT_OUTPUT].setVoltage(10.F);
    } else if (smooth > stepped) {
      outputs[COUPLER_OUTPUT].setVoltage(0.F);
      outputs[COUPLERHOT_OUTPUT].setVoltage(-10.F);
    } else {
      outputs[COUPLER_OUTPUT].setVoltage(0.F);
      outputs[COUPLERHOT_OUTPUT].setVoltage(0.F);
    }
  }
};

struct SSGWidget : ModuleWidget {
  explicit SSGWidget(SSG *module) {
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/SSG.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(
        createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(
        Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(
        box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(11.43, 64.26)), module, SSG::SMOOTHRATEVC_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(57.15, 64.26)), module, SSG::STEPPEDRATEVC_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(11.43, 85.68)), module, SSG::SMOOTHRATE_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(
        mm2px(Vec(57.15, 85.68)), module, SSG::STEPPEDRATE_PARAM));

    addParam(createParamCentered<NKK2>(mm2px(Vec(34.29, 85.68)), module,
                                       SSG::SMOOTHRATEHILO_PARAM));
    addParam(createParamCentered<NKK2>(mm2px(Vec(80.01, 85.68)), module,
                                       SSG::STEPPEDRATEHILO_PARAM));

    addInput(createInputCentered<BlackBananaPort>(mm2px(Vec(11.43, 21.42)),
                                                  module, SSG::SMOOTH_INPUT));
    addInput(createInputCentered<BlackBananaPort>(mm2px(Vec(57.15, 21.42)),
                                                  module, SSG::STEPPED_INPUT));
    addInput(createInputCentered<BlueBananaPort>(
        mm2px(Vec(11.43, 42.84)), module, SSG::SMOOTHRATEVC_INPUT));
    addInput(createInputCentered<BlueBananaPort>(
        mm2px(Vec(57.15, 42.84)), module, SSG::STEPPEDRATEVC_INPUT));
    addInput(createInputCentered<BlueBananaPort>(
        mm2px(Vec(34.29, 64.26)), module, SSG::SMOOTHHOLD_INPUT));
    addInput(createInputCentered<BlueBananaPort>(
        mm2px(Vec(80.01, 64.26)), module, SSG::STEPPEDSAMPLE_INPUT));

    addOutput(createOutputCentered<BlueBananaPort>(mm2px(Vec(34.283, 21.468)),
                                                   module, SSG::SMOOTH_OUTPUT));
    addOutput(createOutputCentered<BlueBananaPort>(
        mm2px(Vec(80.01, 21.42)), module, SSG::STEPPED_OUTPUT));
    addOutput(createOutputCentered<RedBananaPort>(
        mm2px(Vec(34.29, 42.84)), module, SSG::SMOOTHCYCLE_OUTPUT));
    addOutput(createOutputCentered<RedBananaPort>(
        mm2px(Vec(80.01, 42.84)), module, SSG::STEPPEDCYCLE_OUTPUT));
    addOutput(createOutputCentered<RedBananaPort>(mm2px(Vec(34.29, 107.1)),
                                                  module, SSG::COUPLER_OUTPUT));
    addOutput(createOutputCentered<OrangeBananaPort>(
        mm2px(Vec(57.15, 107.1)), module, SSG::COUPLERHOT_OUTPUT));

    addChild(createLightCentered<MediumLight<GreenRedLight>>(
        mm2px(Vec(39.29, 16.42)), module, SSG::SMOOTH_LIGHT));
    addChild(createLightCentered<MediumLight<GreenRedLight>>(
        mm2px(Vec(85.01, 16.42)), module, SSG::STEPPED_LIGHT));
  }
};

Model *modelSSG = createModel<SSG, SSGWidget>("SSG");
