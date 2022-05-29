
#include "SampleInstance.h"

#define NUM_VOICES 4


void SampleInstance::setup(){
    _files = File::getFullPathName(path)
    instruments = {"Sample_1","Sample_2","Sample_3","Sample_4"};
    
    for (int i = 0; i < NUM_VOICES; i++)
        {
            auto* voice = new MySamplerVoice(i);

            switch(i)
            {
            case Sample_1:
                voice->setMidiNote(37);
                break;
            case Sample_2:
                voice->setMidiNote(38);
                break;
            case Sample_3:
                voice->setMidiNote(39);
                break;
            case Sample_4:
                voice->setMidiNote(40);
                break;
                default:
                break;
                }
               addVoice(voice);
        }
    FormatManager.registerBasicFormats();
        for(int i = 0; i < instruments.size();i++){
          setPath(instruments[i]);
        }
}


void SampleInstance::noteOn(int midiChannel,
    int midiNoteNumber,
    float velocity)
{
    const ScopedLock sl(lock);
    
    for(int j = 0; j < getNumSounds(); j++)
    {
        auto* sound = getSound(j).get();
        if(sound->appliesToNote(midiNoteNumber) && sound->appliesToChannel(midiChannel))
        {
            for(int i = 0; i < getNumVoices(); i++)
            {
                if(auto* voice = dynamic_cast<MySamplerVoice*>(getVoice(i)))
                {
                    if(voice->playSound(midiNoteNumber) && voice->getCurrentlyPlayingSound())
                    {
                        stopVoice(voice, 0.0f, false);
                    }
                    startVoice(voice, sound, midiChannel, midiNoteNumber, velocity);
                }
                
            }
       
        }
    }
}

void SampleInstance::setPath(const String &path){
    //I need to change the name of the input files to Sample_1, Sample_2 etc
    int midiNote;
    String instrument;
    
    instrument = path;
    
        if(instrument == String("Sample_1"))
        {
            midiNote = 36;
        
        }

        else if( instrument == String("Sample_2"))
        {
            midiNote = 37;
       
        }
        else if(instrument ==  String("Sample_3"))
        {
            midiNote = 38;
        
        }
        else if (instrument == String("Sample_4"))
        {
            midiNote = 39;
        }
        else
        {
            midiNote = -1;
        }
    
//        juce::File* file(*instrument);
        File* file = new File(_files.getChildFile(instrument));
    
        std::unique_ptr<AudioFormatReader> reader(FormatManager.createReaderFor(*file));
    
        BigInteger note;
    
        note.setBit(midiNote);
        double attackTimeSec = 0.1;
        double releaseTimeSec = 0.1;
        double maxSampleLength = 10.0;
//        String Sample1("Sample_1");
//        String Sample = updateInst(Sample1);
        addSound(new MySamplerSound(instrument, *reader, note, midiNote, attackTimeSec, releaseTimeSec, maxSampleLength));
    
}
    
    
    
//        FileChooser chooser { "Please load a file" };
//
//        if (chooser.browseForFileToOpen())
//        {
//            auto file = chooser.getResult();
//            // the reader can be a local variable here since it's not needed by the SamplerSound after this
//            std::unique_ptr<AudioFormatReader> reader{ FormatManager.createReaderFor(file) };
//            if (reader)
//            {
//                BigInteger note;
//                  note.setBit(midiNote);
//                  double attackTimeSec = 0.1;
//                  double releaseTimeSec = 0.1;
//                  double maxSampleLength = 10.0;
//
//                addSound(new MySamplerSound(instrument, *reader, note, midiNote, attackTimeSec, releaseTimeSec, maxSampleLength));
//            }
//
//        }
    
    
    
    
    
    
    
    


void SampleInstance::loadPath(String instrument){

    
}








       
 





AudioBuffer<float>& SampleInstance::getWaveform()
{
    // get the last added synth sound as a SamplerSound*
    
    auto sound = dynamic_cast<MySamplerSound*>(getSound(getNumSounds()).get());
    if (sound)
    {
        return *sound->getAudioData();
    }
    // just in case it somehow happens that the sound doesn't exist or isn't a SamplerSound,
    // return a static instance of an empty AudioBuffer here...
    static AudioBuffer<float> dummybuffer;
   return dummybuffer;
  
}


//        
String SampleInstance::updateInst(String instrument){
    
    String inst("");
    inst = instrument += 1;
    return inst;
}

int SampleInstance::updateMidiNote(int midiNote){
    int mid = 37;
    midiNote = mid;
    return midiNote;
    
}

    
//    return midiNote;
    
//    FileChooser chooser { "Please load a file" };
//
//    if (chooser.browseForFileToOpen())
//    {
//        auto file = chooser.getResult();
//        // the reader can be a local variable here since it's not needed by the SamplerSound after this
//        std::unique_ptr<AudioFormatReader> reader{ FormatManager.createReaderFor(file) };
//        if (reader)
//        {
//            BigInteger note;
//              note.setBit(midiNote);
//              double attackTimeSec = 0.1;
//              double releaseTimeSec = 0.1;
//              double maxSampleLength = 10.0;
//
//            addSound(new SamplerSound(instrument, *reader, note, midiNote, attackTimeSec, releaseTimeSec, maxSampleLength));
//        }
//
//    }
//
    
    
    
    
    





////
//
//}
//     reader= nullptr;
//        delete file;
//}
//

//
//void SampleInstance::loadFile()
//
//{
//
//    clearSounds();
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
//   addSound(new SamplerSound("Sampler",*reader,range,middleMidiNote,attackTimeSec,releaseTimeSec,maxSampleLength));
//        }
//    }
//}
////
////
////
//
//void SampleInstance::loadFile(const String& path){
//
//    clearSounds();
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
//    addSound(new SamplerSound("Sampler",*reader,range,middleMidiNote, attackTimeSec,releaseTimeSec,maxSampleLength));
////    updateParams();
//    }
//}








void SampleInstance:: parameterChanged (const String& parameterID, float newValue){
    
    
}
