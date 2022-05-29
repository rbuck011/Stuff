/*
  ==============================================================================

    WaveDisplay.h
    Created: 13 Apr 2022 11:42:39pm
    Author:  Rhys William Buckham

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SampleInstance.h"


//==============================================================================
/**
*/
class WaveDisplay  : public Component,
                     public FileDragAndDropTarget
{
public:
    WaveDisplay(NewProjectAudioProcessor& p);
    ~WaveDisplay() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray &files, int x, int y) override;
    String getFileName(){return FileName;}
    bool renameFile (const File& f, const String& newName)  { return f.moveFileTo (f.getSiblingFile(newName));}

private:
    friend class SampleInstance;
    std::vector<float>AudioPoints;
    bool ShouldbePaining{false};
    String FileName{""};
    
    NewProjectAudioProcessor& audioProcessor;
    
    SampleInstance _sample;
    
//    getFile __sample; 
    StringArray instruments;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveDisplay)
};
