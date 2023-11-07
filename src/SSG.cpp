#include "plugin.hpp"

using dsp::SlewLimiter;

struct SSG : Module {
  enum ParamId {
    SMOOTHRATEVC_PARAM,
    STEPPEDRATEVC_PARAM,
    SMOOTHRATE_PARAM,
    STEPPEDRATE_PARAM,
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
  enum LightId { LIGHTS_LEN };

  using volts = float;

  SSG() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    configParam(SMOOTHRATEVC_PARAM, 0.F, 1.F, 0.F, "Smooth Rate VC Attenuator");
    configParam(STEPPEDRATEVC_PARAM, 0.F, 1.F, 0.F,
                "Stepped Rate VC Attenuator");
    configParam(SMOOTHRATE_PARAM, 0.F, 1.F, 0.F, "Smooth Rate");
    configParam(STEPPEDRATE_PARAM, 0.F, 1.F, 0.F, "Stepped Rate");

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

  volts previousOutput{};
  SlewLimiter limiter;

  void process(const ProcessArgs &args) override {
    // The absolute min and max allowed slew rates (in V/s, I hope)
    /* const float slewMax = 1000.F; */
    /* const float slewMin = 0.F; */

    // rate knob all the way left means slow, no change allowed. (0 V/s)
    // all the way right means fast, all changes allowed (infinite V/s?)
    // is that "a lot of slew" or a little?
    float slewParam = params[SMOOTHRATE_PARAM].getValue();
    // rate vc input expects between -5V and 5V, scaled to 0-1.
    // then added to param knob value to get final param.
    // So the knob all the way left
    /* volts slewInput = inputs[SMOOTHRATEVC_INPUT].getVoltage(); */
    /* float slewInputAttenFactor = params[SMOOTHRATEVC_PARAM].getValue(); */
    //
    /* float slew = slewParam + (slewInput * slewInputAttenFactor); */

    float smoothInput = inputs[SMOOTH_INPUT].getVoltage();

    limiter.setRiseFall(slewParam * 75000, slewParam * 75000);
    float smoothVal = limiter.process(args.sampleTime, smoothInput);
    outputs[SMOOTH_OUTPUT].setVoltage(smoothVal);
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

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 21.42)), module,
                                             SSG::SMOOTH_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.15, 21.42)), module,
                                             SSG::STEPPED_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 42.84)), module,
                                             SSG::SMOOTHRATEVC_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.15, 42.84)), module,
                                             SSG::STEPPEDRATEVC_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(34.29, 64.26)), module,
                                             SSG::SMOOTHHOLD_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.01, 64.26)), module,
                                             SSG::STEPPEDSAMPLE_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.283, 21.468)),
                                               module, SSG::SMOOTH_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 21.42)), module,
                                               SSG::STEPPED_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 42.84)), module,
                                               SSG::SMOOTHCYCLE_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 42.84)), module,
                                               SSG::STEPPEDCYCLE_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 107.1)), module,
                                               SSG::COUPLER_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.15, 107.1)), module,
                                               SSG::COUPLERHOT_OUTPUT));
  }
};

Model *modelSSG = createModel<SSG, SSGWidget>("SSG");
