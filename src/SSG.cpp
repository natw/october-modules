#include "plugin.hpp"


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
	enum LightId {
		LIGHTS_LEN
	};

	SSG() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SMOOTHRATEVC_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEPPEDRATEVC_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SMOOTHRATE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEPPEDRATE_PARAM, 0.f, 1.f, 0.f, "");
		configInput(SMOOTH_INPUT, "");
		configInput(STEPPED_INPUT, "");
		configInput(SMOOTHRATEVC_INPUT, "");
		configInput(STEPPEDRATEVC_INPUT, "");
		configInput(SMOOTHHOLD_INPUT, "");
		configInput(STEPPEDSAMPLE_INPUT, "");
		configOutput(SMOOTH_OUTPUT, "");
		configOutput(STEPPED_OUTPUT, "");
		configOutput(SMOOTHCYCLE_OUTPUT, "");
		configOutput(STEPPEDCYCLE_OUTPUT, "");
		configOutput(COUPLER_OUTPUT, "");
		configOutput(COUPLERHOT_OUTPUT, "");
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.43, 64.26)), module, SSG::SMOOTHRATEVC_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(57.15, 64.26)), module, SSG::STEPPEDRATEVC_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.43, 85.68)), module, SSG::SMOOTHRATE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(57.15, 85.68)), module, SSG::STEPPEDRATE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 21.42)), module, SSG::SMOOTH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.15, 21.42)), module, SSG::STEPPED_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 42.84)), module, SSG::SMOOTHRATEVC_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.15, 42.84)), module, SSG::STEPPEDRATEVC_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(34.29, 64.26)), module, SSG::SMOOTHHOLD_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.01, 64.26)), module, SSG::STEPPEDSAMPLE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.283, 21.468)), module, SSG::SMOOTH_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 21.42)), module, SSG::STEPPED_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 42.84)), module, SSG::SMOOTHCYCLE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 42.84)), module, SSG::STEPPEDCYCLE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.29, 107.1)), module, SSG::COUPLER_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.15, 107.1)), module, SSG::COUPLERHOT_OUTPUT));
	}
};


Model* modelSSG = createModel<SSG, SSGWidget>("SSG");