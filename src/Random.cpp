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
		configParam(FLUCRATE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(QUANTSTATES_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STOREDPROB_PARAM, 0.f, 1.f, 0.f, "");
		configInput(FLUCRATECV_INPUT, "");
		configInput(QUANTTRIGGER_INPUT, "");
		configInput(QUANTSTATESCV_INPUT, "");
		configInput(STOREDTRIGGER_INPUT, "");
		configInput(STOREDPROBCV_INPUT, "");
		configOutput(FLUC_OUTPUT, "");
		configOutput(QUANTLIN_OUTPUT, "");
		configOutput(QUANTEXP_OUTPUT, "");
		configOutput(STOREDEVEN_OUTPUT, "");
		configOutput(STOREDDIST_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct RandomWidget : ModuleWidget {
	RandomWidget(Random* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Random.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 21.42)), module, Random::FLUCRATE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 64.26)), module, Random::QUANTSTATES_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.29, 107.1)), module, Random::STOREDPROB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 21.42)), module, Random::FLUCRATECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 42.84)), module, Random::QUANTTRIGGER_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 64.26)), module, Random::QUANTSTATESCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 85.68)), module, Random::STOREDTRIGGER_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.43, 107.1)), module, Random::STOREDPROBCV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(79.897, 21.436)), module, Random::FLUC_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 42.84)), module, Random::QUANTLIN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 64.26)), module, Random::QUANTEXP_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 85.68)), module, Random::STOREDEVEN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.01, 107.1)), module, Random::STOREDDIST_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(84.897, 16.42)), module, Random::FLUC_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(85.01, 37.84)), module, Random::QUANTLIN_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(85.01, 59.26)), module, Random::QUANTEXP_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(85.01, 80.68)), module, Random::STOREDEVEN_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(85.01, 102.1)), module, Random::STOREDDIST_LIGHT));
	}
};


Model* modelRandom = createModel<Random, RandomWidget>("Random");