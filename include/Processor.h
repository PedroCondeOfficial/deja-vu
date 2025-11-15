#pragma once
#include <JuceHeader.h>

class DelayProcessor : public juce::AudioProcessor
{
    public:
        DelayProcessor();
        ~DelayProcessor() override;

        void prepareToPlay(double sampleRate, int samplesPerBlock) override;
        void unload() override;
        void processBuffer(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        void juce::String getName() const override {return 'Deja Vu';}
        bool acceptsMidi() const override {return false;}
        bool producesMidi() const override {return false;}
        bool getTailLengthSeconds() const override {return 2.0;}

        int getNumPrograms() override {return 1;}
        int getCurrentProgram() override {return 0;}
        void setCurrentProgram (int) override {}
        const juce::String getProgramName (int) override {return {};}
        void changeProgramName (int, const juce::String&) override {}

        void getState (juce::MemoryBlock& destData) override;
        void setState (const void* data, int sizeInBytes) override;

        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override {return true;}

        juce::AudioProcessorValueTreeState parameters;

    private:
        std::vector<float> delayBuffer;
        int delayBufferSize = 0;
        int writePosition = 0; 

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayProcessor)
}