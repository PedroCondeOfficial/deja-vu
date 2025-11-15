#include "Processor.h"
#include "Editor.h"

const juce::String delayTimeParamId = "delayTime";
const juce::String feedbackParamId = "feedback";
const juce::String mixParamId = "mix";

DelayProcessor::DelayProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutPut ("Output", juce:AudioChannelSet::stereo(), true)),
        parameters(*this, nullptr, "PARAMETERS",
        {
            std::make_unique<juce::AudioParameterFloat>(
                delayTimeParamId,
                "Delay Time",
                juce::NormalisableRange<float>(0.0f, 2000.0f, 1.0f),
                500.0f,
                "ms"
            ),

            std::make_unique<juce::AudioParameterFloat>(
                feedbackParamId,
                "Feedback",
                juce:NormalisableRange<float>(0.0f, 0.95f, 0.01f),
                "%",
                juce::AudioProcessorParameter::genericParameter,
                [](float value, int) {return juce::String(value * 100.0f, 1); }
            ),

            std::make_unique<juce::AudioParameterFloat>(
                mixParamId,
                "Mix",
                juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                0.5f,
                "%",
                juce::AudioProcessorParameter::genericParameter,
                [](float value, int) {return juce::String(value * 100.0f, 1); }
            )
        })
{
}

DelayProcessor::~DelayProcessor(())

void DelayProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    delayBufferSize = static_cast<int>(sampleRate * 2.0);
    delayBuffer.resize(delayBufferSize * 2);
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
    writePosition = 0;
}

void DelayProcessor::releaseResources()
{
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
}
