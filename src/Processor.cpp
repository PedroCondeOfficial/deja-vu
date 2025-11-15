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

void DelayProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer<float>& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const float delayTimeMs = parameters.getRawParameterValue(delayTimeParamId)->load();
    const float feedbaack = parameters.getRawParameterValue(feedbackParamId)->load();
    const float mix = parameters.getRawParameterValue(mixParamId)->load();

    const float delaySamples = static_cast<int>((delayTimeMs/1000.0f) * getSampleRate());
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int channel = 0; channel < numChannels; channel++) 
    {
        float* channelData = buffer.getWritePointer(channel);
        const int delayBufferChannel = channel % 2;

        for (int sample = 0; sample < numSamples; sample++)
        {
            const float input = channelData[sample];
            int readPosition = writePosition - samples;
            if (readPosition < 0)
            {
                readPosition += delayBufferSize;
            }

            const float delayedSample = delayBuffer[delayBufferChannel * delayBufferSize + readPosition];
            delayBuffer[delayBufferChannel * delayBufferSize + writePosition] = input + (delayedSample * feedback);

            channelData[sample] = (input * (1.0 - mix) + (delayedSample * mix));

            writePosition = (writePosition + 1) % delayBufferSize;
        }
    }

}
