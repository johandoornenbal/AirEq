#include "../Source/SpectrumDisplay.h"
#include "../Source/EQControls.h"
#include "../Source/OutputMeter.h"
#include <iostream>
#include <stdexcept>
void check(bool value,const char* message) { if(!value) throw std::runtime_error(message); }
juce::RangedAudioParameter& parameter(juce::AudioProcessor& p,const juce::String& id) {
    for(auto* v:p.getParameters()) if(auto* r=dynamic_cast<juce::RangedAudioParameter*>(v);r && r->paramID==id) return *r;
    throw std::runtime_error("Missing parameter");
}
void set(juce::AudioProcessor& p,const juce::String& id,float value) { auto& v=parameter(p,id); v.setValueNotifyingHost(v.convertTo0to1(value)); }
void calibration() {
    SpectrumFeed feed; feed.prepare(48000); SpectrumDisplay display(feed);
    for(int i=0;i<SpectrumFeed::size;++i) {
        const float a=.5f*std::sin(juce::MathConstants<float>::twoPi*128.f*static_cast<float>(i)/SpectrumFeed::size);
        feed.push(a,-a,a*.5f,-a*.5f,true);
    }
    display.update(.03);
    check(std::abs(display.levelDb(true,128)+6.0206f)<.02f,"Input calibration or stereo phase cancellation");
    check(std::abs(display.levelDb(false,128)+12.0412f)<.02f,"Compensated calibration");
    display.update(3.); check(display.levelDb(true,128)<=-95.9f,"Stopped spectrum stale");
    feed.prepare(96000);
    for(int i=0;i<SpectrumFeed::size;++i) feed.push(0,0,0,0,true);
    display.update(.03); check(display.getSampleRate()==96000,"Sample-rate reset failed");
    for(int i=0;i<SpectrumFeed::size*16;++i) feed.push(.1f,-.1f,.2f,-.2f,true);
    display.update(.03); // Full queue drops frames and recovers without blocking.
}
template<class T> void unchangedAudio() {
    std::unique_ptr<juce::AudioProcessor> off(createPluginFilter()),on(createPluginFilter());
    std::unique_ptr<juce::AudioProcessorEditor> editor(on->createEditor()); auto* panel=editor->getChildComponent(0);
    auto* toggle=dynamic_cast<juce::TextButton*>(panel->findChildWithID("showSpectrum"));
    toggle->setToggleState(true,juce::dontSendNotification); toggle->onClick();
    for(int model=0;model<controls::modelCount;++model) {
        for(auto* p:{off.get(),on.get()}) {
            set(*p,"model",static_cast<float>(model));
            set(*p,controls::inputId(model),2.f); set(*p,controls::outputId(model),-3.f);
            p->setProcessingPrecision(std::is_same_v<T,float>?juce::AudioProcessor::singlePrecision:juce::AudioProcessor::doublePrecision);
            p->setRateAndBufferSizeDetails(48000,513); p->prepareToPlay(48000,513);
        }
        juce::MidiBuffer midi;
        for(int block=0;block<30;++block) {
            juce::AudioBuffer<T> a(2,513),b(2,513);
            for(int c=0;c<2;++c) for(int i=0;i<513;++i) a.setSample(c,i,static_cast<T>(.2*std::sin((block*513+i)*.07*(c+1))));
            b.makeCopyOf(a); off->processBlock(a,midi); on->processBlock(b,midi);
            for(int c=0;c<2;++c) for(int i=0;i<513;++i) check(a.getSample(c,i)==b.getSample(c,i),"Enabling spectrum changed audio samples");
        }
        off->releaseResources(); on->releaseResources();
    }
}
void screenshot(juce::AudioProcessorEditor& editor,const char* filename) {
    auto image=editor.createComponentSnapshot(editor.getLocalBounds());
    auto stream=juce::File::getCurrentWorkingDirectory().getChildFile(filename).createOutputStream();
    check(stream!=nullptr,"Cannot create preview"); stream->setPosition(0); stream->truncate();
    check(juce::PNGImageFormat().writeImageToStream(image,*stream),"Cannot render preview");
}
void routingAndPreview() {
    std::unique_ptr<juce::AudioProcessor> p(createPluginFilter());
    juce::MemoryBlock bytes; p->getStateInformation(bytes);
    auto xml=juce::AudioProcessor::getXmlFromBinary(bytes.getData(),static_cast<int>(bytes.getSize()));
    const auto prefix=controls::prefix(0); xml->setAttribute(prefix+"hasMatch",true); xml->setAttribute(prefix+"matchDb",-6.0205999);
    xml->setAttribute(prefix+"inputLufs",-18.4); xml->setAttribute(prefix+"outputLufs",-12.3794);
    juce::AudioProcessor::copyXmlToBinary(*xml,bytes); p->setStateInformation(bytes.getData(),static_cast<int>(bytes.getSize()));
    p->setRateAndBufferSizeDetails(48000,256); p->prepareToPlay(48000,256);
    std::unique_ptr<juce::AudioProcessorEditor> editor(p->createEditor()); auto* panel=editor->getChildComponent(0);
    auto* display=dynamic_cast<SpectrumDisplay*>(panel->findChildWithID("spectrum"));
    auto* toggle=dynamic_cast<juce::TextButton*>(panel->findChildWithID("showSpectrum"));
    check(display && toggle && !display->isVisible() && !toggle->getToggleState(),"Spectrum not hidden by default");
    int meters=0; for(auto* child:panel->getChildren()) if(dynamic_cast<OutputMeter*>(child) && child->isVisible()) ++meters;
    check(meters==2,"Default meters hidden");
    toggle->setToggleState(true,juce::dontSendNotification); toggle->onClick(); check(display->isVisible(),"Spectrum switch failed");
    set(*p,controls::originalId(0),1);
    juce::AudioBuffer<float> audio(2,256); juce::MidiBuffer midi;
    for(int trial=0;trial<3;++trial) {
        const int matching=trial==1?0:1;
        set(*p,controls::matchId(0),static_cast<float>(matching));
        set(*p,controls::inputId(0),trial==2?6.0206f:0.f);
        set(*p,controls::outputId(0),trial==2?6.0206f:0.f);
        for(int block=0;block<100;++block) {
            for(int i=0;i<256;++i) { const float a=.5f*std::sin(juce::MathConstants<float>::twoPi*128.f*static_cast<float>(block*256+i)/4096.f); audio.setSample(0,i,a); audio.setSample(1,i,-a); }
            p->processBlock(audio,midi); display->update(256./48000.);
        }
        check(std::abs(display->levelDb(true,128)+6.0206f)<.1f,"Raw input tap incorrect");
        check(std::abs(display->levelDb(false,128)+(trial==2?0.f:matching?12.0412f:6.0206f))<.1f,"Tap not post-compensation/pre-A-B");
    }
    set(*p,controls::inputId(0),0); set(*p,controls::outputId(0),0);
    set(*p,controls::originalId(0),0); set(*p,controls::matchId(0),1);
    set(*p,controls::id(0,0),.7f); set(*p,controls::id(0,2),.35f);
    auto feed=[&] {
        for(int block=0;block<160;++block) {
            for(int i=0;i<256;++i) for(int c=0;c<2;++c) {
                double sample=0;
                for(int band=0;band<120;++band) {
                    const double hz=26.*std::pow(18000./26.,band/119.);
                    sample+=.012*std::pow(hz/100.,-.25)*std::sin((block*256+i)*juce::MathConstants<double>::twoPi*hz/48000.+band*band*.31+c*.37);
                }
                audio.setSample(c,i,static_cast<float>(sample));
            }
            p->processBlock(audio,midi); juce::Timer::callPendingTimersSynchronously();
            if(display->isVisible()) display->update(256./48000.);
        }
    };
    toggle->setToggleState(false,juce::dontSendNotification); toggle->onClick(); feed();
    screenshot(*editor,"meters-preview.png");
    toggle->setToggleState(true,juce::dontSendNotification); toggle->onClick(); feed();
    screenshot(*editor,"spectrum-preview.png");
    check(display->getHeight()>=364,"Spectrum height was not doubled");
    auto* sizes=dynamic_cast<juce::ComboBox*>(panel->findChildWithID("uiSize"));
    juce::ComboBox* models=nullptr;
    for(auto* child:panel->getChildren()) if(auto* combo=dynamic_cast<juce::ComboBox*>(child);combo && combo->getName()=="EQ model") models=combo;
    check(sizes && models,"Layout controls missing");
    for(int model=0;model<controls::modelCount;++model) {
        models->setSelectedId(model+1,juce::sendNotificationSync);
        for(int size:{1,2,3}) {
            sizes->setSelectedId(size,juce::sendNotificationSync);
            const float scale=size==1?.75f:size==3?1.25f:1.f;
            check(editor->getHeight()==juce::roundToInt((controls::hasFrequencies(model)?1012:830)*scale),"Expanded editor height incorrect");
            for(auto* child:panel->getChildren()) if(child->isVisible()) {
                check(panel->getLocalBounds().contains(child->getBounds()),"Control outside expanded editor");
                if(dynamic_cast<juce::Slider*>(child)) check(!child->getBounds().intersects(display->getBounds()),"Spectrum covers a control");
            }
        }
    }
    models->setSelectedId(2,juce::sendNotificationSync); sizes->setSelectedId(2,juce::sendNotificationSync); feed();
    screenshot(*editor,"spectrum-two-row-preview.png");
    toggle->setToggleState(false,juce::dontSendNotification); toggle->onClick();
    check(editor->getHeight()==830,"Meter view did not restore original height");
    toggle->setToggleState(true,juce::dontSendNotification); toggle->onClick();
    int visibleMeters=0; for(auto* child:panel->getChildren()) if(dynamic_cast<OutputMeter*>(child) && child->isVisible()) ++visibleMeters;
    check(visibleMeters==0,"Meters visible over spectrum");
    editor.reset(); editor.reset(p->createEditor());
    check(!editor->getChildComponent(0)->findChildWithID("spectrum")->isVisible(),"New editor did not default to meters");
}
int main() try {
    juce::ScopedJuceInitialiser_GUI init;
    calibration(); unchangedAudio<float>(); unchangedAudio<double>(); routingAndPreview();
    std::cout<<"Spectrum calibration, stereo power, decay, queue saturation, all-model float/double unchanged audio, post-compensation routing and editor previews passed\n";
} catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
