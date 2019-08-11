/*
 * DPF STK Plugins
 * Copyright (C) 2019 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef DPF_STK_FLUTE_HPP_INCLUDED
#define DPF_STK_FLUTE_HPP_INCLUDED

#include "DistrhoPluginUtils.hpp"
#include "../../stk/include/Flute.h"
#include "../../stk/include/Voicer.h"
#include "../../stk/include/SKINImsg.h"

using stk::StkFloat;

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class StkFlute : public Plugin
{
public:
    static const size_t kNumVoices = 6;

    /*
    Control Change Numbers:
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128
    */
    enum Parameters
    {
        kParamJetDelay = 0,
        kParamNoiseGain,
        kParamVibratoFrequency,
        kParamVibratoGain,
        kParamBreathPressure,
        kParamCount
    };

    StkFlute()
        : Plugin(kParamCount, 0, 0), // 0 programs, 0 states
          voicer(0.0)
    {
        for (size_t i=0; i<kParamCount; ++i)
            params[i] = 0.0f;

        for (size_t i=0; i<kNumVoices; ++i)
        {
            flute[i].setSampleRate(getSampleRate());
            voicer.addInstrument(&flute[i]);
        }
    }

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override
    {
        return "STK Flute";
    }

    const char* getDescription() const override
    {
        return "...";
    }

    const char* getMaker() const noexcept override
    {
        return "falkTX, STK";
    }

    const char* getHomePage() const override
    {
        return DISTRHO_PLUGIN_URI;
    }

    const char* getLicense() const noexcept override
    {
        return "ISC";
    }

    uint32_t getVersion() const noexcept override
    {
        return d_version(1, 0, 0);
    }

    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('D', 'F', 'l', 't');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override
    {
        switch (index)
        {
        case kParamJetDelay:
            parameter.hints      = kParameterIsAutomable;
            parameter.name       = "Jet Delay";
            parameter.symbol     = "JetDelay";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamNoiseGain:
            parameter.hints      = kParameterIsAutomable;
            parameter.name       = "Noise Gain";
            parameter.symbol     = "NoiseGain";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamVibratoFrequency:
            parameter.hints      = kParameterIsAutomable;
            parameter.name       = "Vibrato Frequency";
            parameter.symbol     = "VibratoFrequency";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamVibratoGain:
            parameter.hints      = kParameterIsAutomable;
            parameter.name       = "Vibrato Gain";
            parameter.symbol     = "VibratoGain";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamBreathPressure:
            parameter.hints      = kParameterIsAutomable;
            parameter.name       = "Breath Pressure";
            parameter.symbol     = "BreathPressure";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        }
    }

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override
    {
        return params[index];
    }

    void setParameterValue(uint32_t index, float value) override
    {
        switch (index)
        {
        case kParamJetDelay:
            instrumentControlChange(__SK_JetDelay_, value*128.0);
            break;
        case kParamNoiseGain:
            instrumentControlChange(__SK_NoiseLevel_, value*128.0);
            break;
        case kParamVibratoFrequency:
            instrumentControlChange(__SK_ModFrequency_, value*128.0);
            break;
        case kParamVibratoGain:
            instrumentControlChange(__SK_ModWheel_, value*128.0);
            break;
        case kParamBreathPressure:
            instrumentControlChange(__SK_AfterTouch_Cont_, value*128.0);
            break;
        }

        params[index] = value;
    }

    // -------------------------------------------------------------------
    // Process

    void deactivate() override
    {
        for (size_t i=0; i<kNumVoices; ++i)
            flute[i].clear();
    }

    void run(const float**, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount) override
    {
        StkFloat note, velo;
        std::memset(outputs[0], 0, sizeof(float)*frames);

        for (AudioMidiSyncHelper amsh(outputs, frames, midiEvents, midiEventCount); amsh.nextEvent();)
        {
            for (uint32_t i=0; i<amsh.midiEventCount; ++i)
            {
                if (amsh.midiEvents[i].size > MidiEvent::kDataSize)
                    continue;

                const uint8_t*  data = amsh.midiEvents[i].data;
                const uint8_t status = data[0] & 0xF0;

                switch (status)
                {
                case 0x90:
                    note = static_cast<StkFloat>(data[1]);
                    velo = static_cast<StkFloat>(data[2]);
                    if (velo > 0)
                    {
                        voicer.noteOn(note, velo);
                        break;
                    }
                    // fall through
                case 0x80:
                    note = static_cast<StkFloat>(data[1]);
                    velo = static_cast<StkFloat>(data[2]);
                    voicer.noteOff(note, velo);
                    break;
                }

                // TODO pitchbend
                // TODO all notes off
            }

            float* const out = amsh.outputs[0];

            for (uint32_t i=0; i<amsh.frames; ++i)
                out[i] = voicer.tick();
        }
    }

    // -------------------------------------------------------------------
    // ...

    void sampleRateChanged(const double newSampleRate) override
    {
        for (size_t i=0; i<kNumVoices; ++i)
            flute[i].setSampleRate(newSampleRate);
    }

    // -------------------------------------------------------------------

private:
    float params[kParamCount];
    stk::Flute flute[kNumVoices];
    stk::Voicer voicer;

    void instrumentControlChange(int number, StkFloat value)
    {
        for (size_t i=0; i<kNumVoices; ++i)
            flute[i].controlChange(number, value);
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StkFlute)
};

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new StkFlute();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif  // DPF_STK_FLUTE_HPP_INCLUDED
