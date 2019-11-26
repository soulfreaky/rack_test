#include "plugin.hpp"
#include <math.h>


struct Test_1 : Module {
	float phase = 0.f;
    float blinkPhase = 0.f;
	float computeTri(float ph){
		if (ph < 0.f){
			return (1 + ph*4);
		}else{
			return (1 - ph*4);
		}
	}
	float computeSquare(float ph){
		if (ph > 0.f) {
			return 1.f;
		}else{
			return  -1.f;
		}
	}
	float computeSaw(float ph){
		return 2*ph;
	}
	float computeSin(float ph){
		return std::sin(2.f * M_PI * ph);
	}
	float computeExp(float ph){
		return 2*(std::exp(ph) - 1);
	}
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
		SAW_OUTPUT,
		TRI_OUTPUT,
		SQ_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	Test_1() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PITCH_PARAM, 0.f, 6.f, 1.0f, "");
	}


	void process(const ProcessArgs &args) override {
        // Compute the frequency from the pitch parameter and input
        float pitch = params[PITCH_PARAM].getValue();
        pitch += inputs[PITCH_INPUT].getVoltage();
        // The default pitch is C1 = 16.352ff
        float freq = 16.352f * std::pow(2.f, pitch);

        // Accumulate the phase
        phase += freq * args.sampleTime;
        if (phase >= 0.5f)
            phase -= 1.f;



        // Audio signals are typically +/-5V
        // https://vcvrack.com/manual/VoltageStandards.html
        outputs[SAW_OUTPUT].setVoltage(5.f * computeSaw(phase));
		outputs[SQ_OUTPUT].setVoltage(5.f * computeSquare(phase));
		outputs[TRI_OUTPUT].setVoltage(5.f * computeTri(phase));
		outputs[SINE_OUTPUT].setVoltage(5.f * computeSin(phase));
		//outputs[SINE_OUTPUT].setVoltage(5.f * computeExp(phase));

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

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10, 102.713)), module, Test_1::SINE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22, 102.713)), module, Test_1::SAW_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10, 115.713)), module, Test_1::TRI_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22, 115.713)), module, Test_1::SQ_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 25.81)), module, Test_1::BLINK_LIGHT));
	}
};


Model* modelTest_1 = createModel<Test_1, Test_1Widget>("test_1");