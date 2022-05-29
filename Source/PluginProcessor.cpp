
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
static String doubleToString(double val) { return String(val); }
static double stringToDouble(String s){return std::stod(s.toStdString());}

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations  //init list
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),undoManager(new UndoManager())
                         ,APVTS(new AudioProcessorValueTreeState(*this,undoManager.get()))
//                   
                        
#endif
{   //allows to use wavs mp3s etc
//    FormatManager.registerBasicFormats();
    //listening for changes in the value tree
//    APVTS.state.addListener(this);
    //create the voices for my sampler and add them
    
    mySampler.setup();
    using Parameter = AudioProcessorValueTreeState::Parameter;

    for(int i = 0; i < mySampler.getNumVoices(); i++){
        APVTS->createAndAddParameter(std::make_unique<Parameter>(String("ATTACK" + String(i)), "Attack","", juce::NormalisableRange<float> (100.0f, 500.0f), 0.0f, doubleToString, stringToDouble));
        APVTS->createAndAddParameter(std::make_unique<Parameter>(String("DECAY"+ String(i)), "Decay","", juce::NormalisableRange<float> (0.0f, 1000.0f), 0.0f, doubleToString, stringToDouble));
        APVTS->createAndAddParameter(std::make_unique<Parameter>(String("SUSTAIN"+ String(i)), "Sustain","", juce::NormalisableRange<float>(0.0f, 500.0f), 0.0f, doubleToString, stringToDouble));
        APVTS->createAndAddParameter(std::make_unique<Parameter>(String("RELEASE"+String(i)), "Release","", juce::NormalisableRange<float> (0.0f, 5000.0f), 1000.0f, doubleToString, stringToDouble));
        APVTS->createAndAddParameter(std::make_unique<Parameter>(String("FILTERCHOICE"+String(i)), "Type","", juce::NormalisableRange<float> (0.0f, 5000.0f), 1000.0f, doubleToString, stringToDouble));
        APVTS->createAndAddParameter(std::make_unique<Parameter>(String("CUTOFF"+String(i)), "Cutoff","", juce::NormalisableRange<float> (0.0f, 5000.0f), 1000.0f, doubleToString, stringToDouble));
        APVTS->createAndAddParameter(std::make_unique<Parameter>(String("RESONANCE"+String(i)), "Resonance","", juce::NormalisableRange<float> (0.0f, 5000.0f), 1000.0f, doubleToString, stringToDouble));
        
         if(auto* voice = dynamic_cast<MySamplerVoice*>(mySampler.getVoice(i))){
            APVTS->addParameterListener(String("ATTACK" + String(i)),voice);
            APVTS->addParameterListener(String("DECAY" + String(i)),voice);
            APVTS->addParameterListener(String("SUSTAIN" + String(i)),voice);
            APVTS->addParameterListener(String("RELEASE" + String(i)),voice);
             APVTS->addParameterListener(String("FILTERCHOICE" + String(i)),voice);
             APVTS->addParameterListener(String("CUTOFF" + String(i)),voice);
             APVTS->addParameterListener(String("RESONANCE" + String(i)),voice);
            }


    }
    APVTS->state = ValueTree("PARAMETERS");
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
//    FormatReader = nullptr;
//    APVTS.state.removeListener(this);
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;  
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{


    mySampler.setCurrentPlaybackSampleRate(sampleRate);
    updateParams();

}



void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    if(ShouldUpdate){
        updateParams();
    }
    
    mySampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor(*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
       MemoryOutputStream stream(destData, false);
        APVTS->state.writeToStream(stream);
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
   
    ValueTree tree = ValueTree::readFromData(data, sizeInBytes);

    if(tree.isValid())
    {
        if(tree.hasType("PARAMETERS"))
        {
            APVTS->state = tree;
        }
    }
    
    
    
    
    
}
/// Load The File
//void NewProjectAudioProcessor::loadFile()
//
//{
//
//    mySampler.clearSounds();
//    //Opens up a browser to select sounds
//    FileChooser chooser{"choose a file"};
//
//    //result of the file
//    if(chooser.browseForFileToOpen()){
//        //the result result = the file.
//        auto result = chooser.getResult();
//        //create a reader for the file
////        FormatReader = FormatManager.createReaderFor(result);
//        
//    std::unique_ptr<AudioFormatReader>reader{FormatManager.createReaderFor(result)};
//        if(reader){
//    //////// Play The File
//    
//
//    int middleMidiNote = 60;
//    double attackTimeSec = 0.1;
//    double releaseTimeSec = 0.1;
//    double maxSampleLength = 10.0;
//     
//    BigInteger range;
//    range.setRange(0, 128, true);
//    // create new samplersound that contains the audio loaded (Format Reader)
//            
//            
//    mySampler.addSound(new MySamplerSound("Sampler",*reader,range,middleMidiNote,attackTimeSec,releaseTimeSec,maxSampleLength));
//        }
//    }
//}
////
////
////
//
//void NewProjectAudioProcessor::loadFile(const String& path){
//
//    mySampler.clearSounds();
//
//    auto file = File(path);
//    FormatReader = FormatManager.createReaderFor(file);
//    std::unique_ptr<AudioFormatReader> reader{FormatManager.createReaderFor(file) };
//
//    if(reader){
//    BigInteger range;
//    range.setRange(0, 128, true);
//    int middleMidiNote = 60;
//    double attackTimeSec = 0.1;
//    double releaseTimeSec = 0.1;
//    double maxSampleLength = 10.0;
////
////
////    auto sample = std::unique_ptr<WaveSamplerData> (new WaveSamplerData (*reader, 10.0));
//
//    // create new samplersound that contains the audio loaded (Format Reader)
//
////   for(int i = 0; i < mySampler.getNumVoices(); i++){
//
//    mySampler.addSound(new MySamplerSound("Sampler",*reader,range,middleMidiNote, attackTimeSec,releaseTimeSec,maxSampleLength));
////    updateParams();
////   }
//    }
//}
//
//AudioBuffer<float>& NewProjectAudioProcessor::getWaveform()
//{
//    // get the last added synth sound as a SamplerSound*
//    auto sound = dynamic_cast<MySamplerSound*>(mySampler.getSound(mySampler.getNumSounds() - 1).get());
//    if (sound)
//    {
//        return *sound->getAudioData();
//    }
//    // just in case it somehow happens that the sound doesn't exist or isn't a SamplerSound,
//    // return a static instance of an empty AudioBuffer here...
//    static AudioBuffer<float> dummybuffer;
//    return dummybuffer;
//}



void NewProjectAudioProcessor::updateParams(){
    updateADSRParams();
//    updateFilterParams();
}


void NewProjectAudioProcessor::updateADSRParams()
{


   ShouldUpdate = false;


    for(int i = 0; i < mySampler.getNumSounds();++i){

        if(auto voice = dynamic_cast<MySamplerVoice*>(mySampler.getVoice(i))){

            auto& attack = *APVTS->getRawParameterValue("ATTACK"+String(i));
            auto& decay = *APVTS->getRawParameterValue("DECAY"+String(i));
            auto& sustain = *APVTS->getRawParameterValue("SUSTAIN"+String(i));
            auto& release = *APVTS->getRawParameterValue("RELEASE"+String(i));

            voice->updateAttack(attack.load());
            voice->updateDecay(decay.load());
            voice->updateSustain(sustain.load());
            voice->updateRelease(release.load());

          }

    }
}

void NewProjectAudioProcessor::updateFilterParams(){
    
    ShouldUpdate = false;
    
    for(int i = 0; i < mySampler.getNumSounds();++i){

        if(auto voice = dynamic_cast<MySamplerVoice*>(mySampler.getVoice(i))){

           auto& filterchoice = *APVTS->getRawParameterValue("FILTERCHOICE"+String(i));
            auto& freq = *APVTS->getRawParameterValue("CUTOFF"+String(i));
            auto& res = *APVTS->getRawParameterValue("RESONANCE"+String(i));
    
            voice->updateType(filterchoice.load());
            voice->updateCutoff(freq.load());
            voice->updateRes(res.load());
            
        }

  }
            
}
void NewProjectAudioProcessor::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    ShouldUpdate = true;
}


void NewProjectAudioProcessor::updateADSR(const float attack, const float decay, const float sustain, const float release){
//    adsrData.update(attack,decay,sustain,release);
}



void NewProjectAudioProcessor::setType(const int filtertype, const float cutoff, const float res){
     
//    filterData.setParams(filtertype, cutoff, res);
  
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}


