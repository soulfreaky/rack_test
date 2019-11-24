#include "plugin.hpp"
#include <math.h>


struct Test_1 : Module {
	float phase = 0.f;
    float blinkPhase = 0.f;
	enum ParamIds {
		PITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SINE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	Test_1() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PITCH_PARAM, 0.f, 4.f, 1.0f, "");
	}

	void process(const ProcessArgs &args) override {
        // Compute the frequency from the pitch parameter and input
        float pitch = params[PITCH_PARAM].getValue();
        pitch += inputs[PITCH_INPUT].getVoltage();
        //pitch = clamp(pitch, -1000.f, 1000.f);
        // The default pitch is C4 = 261.6256f
        float freq = 30.00f * std::pow(2.f, pitch);

        // Accumulate the phase
        phase += freq * args.sampleTime;
        if (phase >= 0.5f)
            phase -= 1.f;

        // Compute the saw output
        float saw = phase;
        // Audio signals are typically +/-5V
        // https://vcvrack.com/manual/VoltageStandards.html
        outputs[SINE_OUTPUT].setVoltage(5.f * saw);

        // Blink light at 1Hz
        blinkPhase += args.sampleTime*2;
        if (blinkPhase >= 1.f)
            blinkPhase -= 1.f;
        lights[BLINK_LIGHT].setBrightness(blinkPhase);
    }
};


struct Test_1Widget : ModuleWidget {
	Test_1Widget(Test_1* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/test_1.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 46.063)), module, Test_1::PITCH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 77.478)), module, Test_1::PITCH_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 108.713)), module, Test_1::SINE_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 25.81)), module, Test_1::BLINK_LIGHT));
	}
};


Model* modelTest_1 = createModel<Test_1, Test_1Widget>("test_1");