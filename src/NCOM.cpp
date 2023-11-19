#include "plugin.hpp"

struct NCOM : Module {
  enum ParamId {
    POS_IN_ATTEN_PARAM,
    SENSE_PARAM,
    N_PARAM,
    N_CV_ATTEN_PARAM,
    PARAMS_LEN
  };
  enum InputId {
    POSITIVE_INPUT,
    NEGATIVE_INPUT,
    N_CV_INPUT,
    INPUTS_LEN
  };
  enum OutputId {
    COMP_OUTPUT,
    DIV_N_OUTPUT,
    STEPPED_OUTPUT,
    OUTPUTS_LEN
  };
  enum LightId {
    LIGHTS_LEN
  };

  NCOM() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

    configInput(POSITIVE_INPUT, "Positive");
    configParam(POS_IN_ATTEN_PARAM, 0.F, 1.F, 0.F, "Input Atten.", "%", 0.F, 100.F);
    configInput(NEGATIVE_INPUT, "Negative");
    configParam(SENSE_PARAM, -5.F, 5.F, 0.F, "Sense");
    configOutput(COMP_OUTPUT, "Comparator");

    configParam(N_PARAM, 1.F, 31.F, 1.F, "N");
    getParamQuantity(N_PARAM)->snapEnabled = true;
    configParam(N_CV_ATTEN_PARAM, 0.F, 1.F, 0.F, "N VC Atten.", "%", 0.F, 100.F);
    configInput(N_CV_INPUT, "N VC");
    configOutput(DIV_N_OUTPUT, "Divided");
    configOutput(STEPPED_OUTPUT, "Stepped");
  }

  static constexpr float V_SEMITONE = 1.F / 12.F;

  float step = 0.F;
  dsp::SchmittTrigger compTrigger;
  dsp::ClockDivider divider;
  dsp::PulseGenerator compPulse;
  dsp::PulseGenerator divPulse;

  void process(const ProcessArgs& args) override {
    float n_param = params[N_PARAM].getValue();
    float n_cv = inputs[N_CV_INPUT].getVoltage() * params[N_CV_ATTEN_PARAM].getValue();
    float n = math::clamp(n_param + (n_cv * 2.F), 1.F, 31.F);

    divider.setDivision(static_cast<unsigned int>(n));

    float posInput = inputs[POSITIVE_INPUT].getVoltage() * params[POS_IN_ATTEN_PARAM].getValue();
    float sense = params[SENSE_PARAM].getValue();
    posInput += sense;

    float negInput = inputs[NEGATIVE_INPUT].getVoltage();

    bool gt = posInput > negInput;
    float compOut = gt ? 10.F : 0.F;

    outputs[STEPPED_OUTPUT].setVoltage(step * V_SEMITONE);

    bool pulse = compTrigger.process(compOut, 4.F, 2.F);
    if (pulse) {
      compPulse.trigger();
      if (divider.process()) {
        divPulse.trigger();
        step = 0.F;
      } else {
        step += 1.F;
      }
    }

    outputs[COMP_OUTPUT].setVoltage(compOut);
    outputs[DIV_N_OUTPUT].setVoltage(divPulse.process(args.sampleTime) ? 10.F : 0.F);
  }
};

struct NCOMWidget : ModuleWidget {
  explicit NCOMWidget(NCOM* module) {
    setModule(module);
    setPanel(createPanel(asset::plugin(pluginInstance, "res/NCOM.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(
        Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.43, 21.42)), module,
                                                 NCOM::POS_IN_ATTEN_PARAM));
    addParam(
        createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 21.42)), module, NCOM::SENSE_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 107.1)), module, NCOM::N_PARAM));
    addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.43, 85.68)), module,
                                                 NCOM::N_CV_ATTEN_PARAM));

    addInput(createInputCentered<BlackBananaPort>(mm2px(Vec(11.43, 42.84)), module,
                                                  NCOM::POSITIVE_INPUT));
    addInput(createInputCentered<BlackBananaPort>(mm2px(Vec(11.43, 64.26)), module,
                                                  NCOM::NEGATIVE_INPUT));
    addInput(
        createInputCentered<BlueBananaPort>(mm2px(Vec(11.43, 107.1)), module, NCOM::N_CV_INPUT));

    addOutput(
        createOutputCentered<RedBananaPort>(mm2px(Vec(34.29, 42.84)), module, NCOM::COMP_OUTPUT));
    addOutput(
        createOutputCentered<RedBananaPort>(mm2px(Vec(34.29, 64.26)), module, NCOM::DIV_N_OUTPUT));
    addOutput(createOutputCentered<BlackBananaPort>(mm2px(Vec(34.29, 85.68)), module,
                                                    NCOM::STEPPED_OUTPUT));
  }
};

Model* modelNCOM = createModel<NCOM, NCOMWidget>("NCOM");
