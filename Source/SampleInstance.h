/*
  ==============================================================================

    SampleInstance.h
    Created: 24 May 2022 3:58:58pm
    Author:  Rhys William Buckham

  ==============================================================================
*/

#pragma once
#include "MySamplerSound.h"
#include "MySamplerVoice.h"



class SampleInstance : public Synthesiser, public AudioProcessorValueTreeState::Listener
                       
{
    

    StringArray instruments; 
    
public:
    void setup();
//    void update(const String& path);
    void parameterChanged (const String& parameterID, float newValue)override;
//    AudioBuffer<float>& getWaveform();
//    void loadFile(const String& path);
    void noteOn(int midiChannel,
                int midiNoteNumber,
                float velocity);
//    void loadFile();
    String updateInst(String instrument);
    int updateMidiNote(int midiNote);
//    void setPath(const String& path);
    void setPath(const String &path);
    void loadPath(String instrument);
    AudioBuffer<float>& getWaveform();
//    String setp(String file);
private:
//    String instrument;
//    friend class WaveDisplay;
    
    typedef enum Sounds {Sample_1 = 0, Sample_2, Sample_3,Sample_4} Sounds_t;
    
    AudioFormatManager FormatManager;
    File _files;
    AudioFormatReader* FormatReader{nullptr};
//    String& PPath;
    
};

