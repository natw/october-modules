#include "plugin.hpp"


struct SSG : Module {
	enum ParamId {
		SMOOTHRATEVCPARAM_PARAM,
		STEPPEDRATEVCPARAM_PARAM,
		SMOOTHRATEINPUT_PARAM,
		STEPPEDRATEINPUT_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SMOOTHINPUT_INPUT,
		STEPPEDINPUT_INPUT,
		SMOOTHRATEVCINPUT_INPUT,
		STEPPEDRATEVCINPUT_INPUT,
		SMOOTHHOLDINPUT_INPUT,
		STEPPEDSAMPLEINPUT_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		SMOOTHOUTPUT_OUTPUT,
		STEPPEDOUTPUT_OUTPUT,
		SMOOTH_CYCLE_OUT_OUTPUT,
		STEPPEDCYCLEOUTPUT_OUTPUT,
		COUPLEROUTPUT_OUTPUT,
		COUPLEHOTOUTPUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	SSG() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SMOOTHRATEVCPARAM_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEPPEDRATEVCPARAM_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SMOOTHRATEINPUT_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEPPEDRATEINPUT_PARAM, 0.f, 1.f, 0.f, "");
		configInput(SMOOTHINPUT_INPUT, "");
		configInput(STEPPEDINPUT_INPUT, "");
		configInput(SMOOTHRATEVCINPUT_INPUT, "");
		configInput(STEPPEDRATEVCINPUT_INPUT, "");
		configInput(SMOOTHHOLDINPUT_INPUT, "");
		configInput(STEPPEDSAMPLEINPUT_INPUT, "");
		configOutput(SMOOTHOUTPUT_OUTPUT, "");
		configOutput(STEPPEDOUTPUT_OUTPUT, "");
		configOutput(SMOOTH_CYCLE_OUT_OUTPUT, "");
		configOutput(STEPPEDCYCLEOUTPUT_OUTPUT, "");
		configOutput(COUPLEROUTPUT_OUTPUT, "");
		configOutput(COUPLEHOTOUTPUT_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct SSGWidget : ModuleWidget {
	SSGWidget(SSG* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SSG.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.43, 64.26)), module, SSG::SMOOTHRATEVCPARAM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(57.15, 64.26)), module, SSG::STEPPEDRATEVCPARAM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.43, 85.68)), module, SSG::SMOOTHRATEINPUT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(57.15, 85.68)), module, SSG::STEPPEDRATEINPUT_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 21.42)), module, SSG::SMOOTHINPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.15, 21.42)), module, SSG::STEPPEDINPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 42.84)), module, SSG::SMOOTHRATEVCINPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.15, 42.84)), module, SSG::STEPPEDRATEVCINPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(34.29, 64.26)), module, SSG::SMOOTHHOLDINPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.01, 64.26)), module, SSG::STEPPEDSAMPLEINPUT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.283, 21.468)), module, SSG::SMOOTHOUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 21.42)), module, SSG::STEPPEDOUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 42.84)), module, SSG::SMOOTH_CYCLE_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 42.84)), module, SSG::STEPPEDCYCLEOUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 107.1)), module, SSG::COUPLEROUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.15, 107.1)), module, SSG::COUPLEHOTOUTPUT_OUTPUT));
	}
};


Model* modelSSG = createModel<SSG, SSGWidget>("SSG");