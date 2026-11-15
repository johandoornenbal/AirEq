#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>

// Single audio producer / editor consumer. FFT and drawing run on the UI thread.
// Full queues drop frames instead of blocking or allocating in the callback.
class SpectrumFeed {
public:
    static constexpr int order=12, size=1<<order, hop=size/2, slots=5;
    struct Frame {
        std::array<std::array<float,size>,4> channels{};
        double rate=48000;
        unsigned generation=0;
        bool hasReference=false;
    };
    void prepare(double sampleRate) { rate=sampleRate; filled=0; ++generation; }
    // Called only by the audio producer; toggling discards a partial old frame.
    void setActive(bool value) { if(value!=active) { active=value; filled=0; ++generation; } }
    void push(float refL,float refR,float outL,float outR,bool hasReference) {
        partial.channels[0][static_cast<size_t>(filled)]=refL;
        partial.channels[1][static_cast<size_t>(filled)]=refR;
        partial.channels[2][static_cast<size_t>(filled)]=outL;
        partial.channels[3][static_cast<size_t>(filled)]=outR;
        if(++filled<size) return;
        partial.rate=rate; partial.generation=generation; partial.hasReference=hasReference;
        const auto w=write.load(std::memory_order_relaxed),next=(w+1)%slots;
        if(next!=read.load(std::memory_order_acquire)) {
            frames[static_cast<size_t>(w)]=partial;
            write.store(next,std::memory_order_release);
        }
        for(auto& channel:partial.channels) std::move(channel.begin()+hop,channel.end(),channel.begin());
        filled=hop;
    }
    bool pop(Frame& frame) {
        const auto r=read.load(std::memory_order_relaxed);
        if(r==write.load(std::memory_order_acquire)) return false;
        frame=frames[static_cast<size_t>(r)]; read.store((r+1)%slots,std::memory_order_release); return true;
    }
private:
    std::array<Frame,slots> frames{};
    Frame partial;
    std::atomic<int> write{0},read{0};
    bool active=false;
    int filled=0;
    unsigned generation=0;
    double rate=48000;
};

class SpectrumDisplay final : public juce::Component {
public:
    static constexpr int bins=SpectrumFeed::size/2+1;
    explicit SpectrumDisplay(SpectrumFeed& source):feed(source) {
        setComponentID("spectrum"); setTitle("Input and compensated spectrum");
        setDescription("Gold: input before processing. Teal: processed signal after compensation, before A/B. Stereo power average, Hann window, logarithmic frequency, dBFS peak amplitude.");
        reference.fill(floorDb); compensated.fill(floorDb);
        for(int i=0;i<SpectrumFeed::size;++i) {
            window[static_cast<size_t>(i)]=.5f-.5f*std::cos(juce::MathConstants<float>::twoPi*static_cast<float>(i)/static_cast<float>(SpectrumFeed::size-1));
            windowSum+=window[static_cast<size_t>(i)];
        }
        clear();
    }
    void clear() {
        while(feed.pop(frame)) {}
        reference.fill(floorDb); compensated.fill(floorDb); initialized=false; idle=0; repaint();
    }
    void update(double elapsed) {
        bool received=false;
        while(feed.pop(frame)) {
            if(frame.generation!=generation || frame.rate!=sampleRate) {
                reference.fill(floorDb); compensated.fill(floorDb); initialized=false;
                generation=frame.generation; sampleRate=frame.rate;
            }
            analyse(frame); received=true;
        }
        if(received) idle=0;
        else {
            idle+=elapsed;
            if(idle>.3) {
                const auto fall=static_cast<float>(elapsed*65.);
                for(auto* values:{&reference,&compensated}) for(auto& db:*values) db=std::max(floorDb,db-fall);
            }
        }
        repaint();
    }
    float levelDb(bool original,int bin) const { return (original?reference:compensated)[static_cast<size_t>(bin)]; }
    double getSampleRate() const { return sampleRate; }
    void paint(juce::Graphics& g) override {
        const juce::Colour gold{0xffd7ac61},teal{0xff72cfbd},cream{0xfff4eddb};
        g.setColour(juce::Colour(0xff141e19)); g.fillRoundedRectangle(getLocalBounds().toFloat(),8);
        g.setFont(juce::FontOptions(11.f)); g.setColour(cream);
        g.drawText("FREQUENCY SPECTRUM",16,10,200,20,juce::Justification::left);
        g.setColour(gold); g.fillEllipse(static_cast<float>(getWidth()-321),16,6,6);
        g.drawText("Input",getWidth()-308,9,95,22,juce::Justification::left);
        g.setColour(teal); g.fillEllipse(static_cast<float>(getWidth()-207),16,6,6);
        g.drawText("After LUFS compensation",getWidth()-194,9,180,22,juce::Justification::left);
        const auto plot=plotBounds();
        g.setFont(juce::FontOptions(10.f));
        for(int db:{12,0,-24,-48,-72,-96}) {
            const float y=yFor(static_cast<float>(db),plot);
            g.setColour(cream.withAlpha(.09f)); g.drawHorizontalLine(juce::roundToInt(y),plot.getX(),plot.getRight());
            g.setColour(cream.withAlpha(.5f)); g.drawText(juce::String(db),0,juce::roundToInt(y)-7,40,14,juce::Justification::right);
        }
        for(float hz:{20.f,50.f,100.f,200.f,500.f,1000.f,2000.f,5000.f,10000.f,20000.f}) {
            if(hz>maximumFrequency()) continue;
            const float x=xFor(hz,plot);
            g.setColour(cream.withAlpha(.07f)); g.drawVerticalLine(juce::roundToInt(x),plot.getY(),plot.getBottom());
            g.setColour(cream.withAlpha(.5f));
            g.drawText(hz>=1000?juce::String(hz/1000,0)+"k":juce::String(hz,0),juce::roundToInt(x)-18,juce::roundToInt(plot.getBottom())+5,36,16,juce::Justification::centred);
        }
        g.saveState(); g.reduceClipRegion(plot.toNearestInt());
        drawCurve(g,reference,gold,plot); drawCurve(g,compensated,teal,plot); g.restoreState();
        g.setColour(cream.withAlpha(.48f)); g.setFont(juce::FontOptions(10.f));
        g.drawText("Hz  /  dBFS    /    Stereo power average    /    4096-point FFT",16,getHeight()-25,getWidth()-32,17,juce::Justification::left);
        if(!hasReference) {
            g.setColour(gold); g.drawText("Input unavailable",plot.toNearestInt(),juce::Justification::centred);
        }
    }
private:
    static constexpr float floorDb=-96.f;
    void analyse(const SpectrumFeed::Frame& input) {
        std::array<std::array<float,bins>,2> power{};
        for(int channel=0;channel<4;++channel) {
            fftData.fill(0);
            for(int i=0;i<SpectrumFeed::size;++i) {
                const auto value=input.channels[static_cast<size_t>(channel)][static_cast<size_t>(i)];
                fftData[static_cast<size_t>(i)]=(std::isfinite(value)?value:0.f)*window[static_cast<size_t>(i)];
            }
            fft.performFrequencyOnlyForwardTransform(fftData.data());
            for(int bin=0;bin<bins;++bin) {
                const auto amplitude=fftData[static_cast<size_t>(bin)]*(bin==0 || bin==bins-1?1.f:2.f)/windowSum;
                power[static_cast<size_t>(channel/2)][static_cast<size_t>(bin)]+=.5f*amplitude*amplitude;
            }
        }
        hasReference=input.hasReference;
        const auto rise=static_cast<float>(1.-std::exp(-SpectrumFeed::hop/(input.rate*.035)));
        const auto fall=static_cast<float>(1.-std::exp(-SpectrumFeed::hop/(input.rate*.22)));
        for(int path=0;path<2;++path) {
            auto& values=path==0?reference:compensated;
            for(int bin=0;bin<bins;++bin) {
                const auto target=path==0 && !hasReference?floorDb:juce::jlimit(floorDb,24.f,10.f*std::log10(std::max(1.e-12f,power[static_cast<size_t>(path)][static_cast<size_t>(bin)])));
                auto& db=values[static_cast<size_t>(bin)];
                const auto coefficient=target>db?rise:fall;
                db=initialized?db+coefficient*(target-db):target;
            }
        }
        initialized=true;
    }
    juce::Rectangle<float> plotBounds() const { return {47,42,static_cast<float>(getWidth()-69),static_cast<float>(getHeight()-100)}; }
    float maximumFrequency() const { return static_cast<float>(std::min(20000.,sampleRate*.5)); }
    float xFor(float hz,juce::Rectangle<float> p) const { return p.getX()+std::log(hz/20.f)/std::log(maximumFrequency()/20.f)*p.getWidth(); }
    static float yFor(float db,juce::Rectangle<float> p) { return p.getBottom()-(db-floorDb)/(12.f-floorDb)*p.getHeight(); }
    void drawCurve(juce::Graphics& g,const std::array<float,bins>& values,juce::Colour colour,juce::Rectangle<float> plot) {
        juce::Path path;
        const int pixels=juce::roundToInt(plot.getWidth());
        for(int pixel=0;pixel<=pixels;++pixel) {
            const double hz=20.*std::pow(maximumFrequency()/20.,static_cast<double>(pixel)/pixels);
            const double position=hz*SpectrumFeed::size/sampleRate;
            const int low=juce::jlimit(1,bins-1,static_cast<int>(position)),high=std::min(bins-1,low+1);
            float db=juce::jmap(juce::jlimit(0.f,1.f,static_cast<float>(position-low)),values[static_cast<size_t>(low)],values[static_cast<size_t>(high)]);
            // Retain narrow peaks when many linear FFT bins share one log pixel.
            const double nextHz=20.*std::pow(maximumFrequency()/20.,static_cast<double>(pixel+1)/pixels);
            for(int bin=low+1;bin<bins && bin<=static_cast<int>(nextHz*SpectrumFeed::size/sampleRate);++bin) db=std::max(db,values[static_cast<size_t>(bin)]);
            const float x=plot.getX()+static_cast<float>(pixel),y=yFor(db,plot);
            if(pixel==0) path.startNewSubPath(x,y); else path.lineTo(x,y);
        }
        g.setColour(colour); g.strokePath(path,juce::PathStrokeType(1.7f));
    }
    SpectrumFeed& feed;
    SpectrumFeed::Frame frame;
    juce::dsp::FFT fft{SpectrumFeed::order};
    std::array<float,SpectrumFeed::size> window{};
    std::array<float,SpectrumFeed::size*2> fftData{};
    std::array<float,bins> reference{},compensated{};
    float windowSum=0;
    double sampleRate=48000,idle=0;
    unsigned generation=0;
    bool initialized=false,hasReference=false;
};
