/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#ifndef JUCE_VST3COMMON_H_INCLUDED
#define JUCE_VST3COMMON_H_INCLUDED

//==============================================================================
#define JUCE_DECLARE_VST3_COM_REF_METHODS \
    Steinberg::uint32 PLUGIN_API addRef() override   { return (Steinberg::uint32) ++refCount; } \
    Steinberg::uint32 PLUGIN_API release() override  { const int r = --refCount; if (r == 0) delete this; return (Steinberg::uint32) r; }

#define JUCE_DECLARE_VST3_COM_QUERY_METHODS \
    Steinberg::tresult PLUGIN_API queryInterface (const Steinberg::TUID, void** obj) override \
    { \
        jassertfalse; \
        *obj = nullptr; \
        return Steinberg::kNotImplemented; \
    }

static bool doUIDsMatch (const Steinberg::TUID a, const Steinberg::TUID b) noexcept
{
    return std::memcmp (a, b, sizeof (Steinberg::TUID)) == 0;
}

#define TEST_FOR_AND_RETURN_IF_VALID(iidToTest, ClassType) \
    if (doUIDsMatch (iidToTest, ClassType::iid)) \
    { \
        addRef(); \
        *obj = dynamic_cast<ClassType*> (this); \
        return Steinberg::kResultOk; \
    }

#define TEST_FOR_COMMON_BASE_AND_RETURN_IF_VALID(iidToTest, CommonClassType, SourceClassType) \
    if (doUIDsMatch (iidToTest, CommonClassType::iid)) \
    { \
        addRef(); \
        *obj = (CommonClassType*) static_cast<SourceClassType*> (this); \
        return Steinberg::kResultOk; \
    }

//==============================================================================
inline juce::String toString (const Steinberg::char8* string) noexcept      { return juce::String (string); }
inline juce::String toString (const Steinberg::char16* string) noexcept     { return juce::String (juce::CharPointer_UTF16 ((juce::CharPointer_UTF16::CharType*) string)); }

// NB: The casts are handled by a Steinberg::UString operator
inline juce::String toString (const Steinberg::UString128& string) noexcept { return toString (static_cast<const Steinberg::char16*> (string)); }
inline juce::String toString (const Steinberg::UString256& string) noexcept { return toString (static_cast<const Steinberg::char16*> (string)); }

inline void toString128 (Steinberg::Vst::String128 result, const char* source)
{
    Steinberg::UString (result, 128).fromAscii (source);
}

inline void toString128 (Steinberg::Vst::String128 result, const juce::String& source)
{
    Steinberg::UString (result, 128).fromAscii (source.toUTF8());
}

inline Steinberg::Vst::TChar* toString (const juce::String& source) noexcept
{
    return reinterpret_cast<Steinberg::Vst::TChar*> (source.toUTF16().getAddress());
}

#if JUCE_WINDOWS
 static const Steinberg::FIDString defaultVST3WindowType = Steinberg::kPlatformTypeHWND;
#else
 static const Steinberg::FIDString defaultVST3WindowType = Steinberg::kPlatformTypeNSView;
#endif


//==============================================================================
static inline Steinberg::Vst::SpeakerArrangement getArrangementForBus (Steinberg::Vst::IAudioProcessor* processor,
                                                                       bool isInput, int busIndex)
{
    Steinberg::Vst::SpeakerArrangement arrangement = Steinberg::Vst::SpeakerArr::kEmpty;

    if (processor != nullptr)
        processor->getBusArrangement (isInput ? Steinberg::Vst::kInput : Steinberg::Vst::kOutput,
                                      (Steinberg::int32) busIndex, arrangement);

    return arrangement;
}

/** For the sake of simplicity, there can only be 1 arrangement type per channel count.
    i.e.: 4 channels == k31Cine OR k40Cine
*/
static inline Steinberg::Vst::SpeakerArrangement getArrangementForNumChannels (int numChannels) noexcept
{
    using namespace Steinberg::Vst::SpeakerArr;

    switch (numChannels)
    {
        case 0:     return kEmpty;
        case 1:     return kMono;
        case 2:     return kStereo;
        case 3:     return k30Cine;
        case 4:     return k31Cine;
        case 5:     return k50;
        case 6:     return k51;
        case 7:     return k61Cine;
        case 8:     return k71CineFullFront;
        case 9:     return k90;
        case 10:    return k91;
        case 11:    return k101;
        case 12:    return k111;
        case 13:    return k130;
        case 14:    return k131;
        case 24:    return (Steinberg::Vst::SpeakerArrangement) 1929904127; // k222
        default:    break;
    }

    jassert (numChannels >= 0);

    juce::BigInteger bi;
    bi.setRange (0, jmin (numChannels, (int) (sizeof (Steinberg::Vst::SpeakerArrangement) * 8)), true);
    return (Steinberg::Vst::SpeakerArrangement) bi.toInt64();
}

static inline Steinberg::Vst::Speaker getSpeakerType (AudioChannelSet::ChannelType type) noexcept
{
    using namespace Steinberg::Vst;

    switch (type)
    {
        case AudioChannelSet::left:              return kSpeakerL;
        case AudioChannelSet::right:             return kSpeakerR;
        case AudioChannelSet::centre:            return kSpeakerC;
        case AudioChannelSet::subbass:           return kSpeakerLfe;
        case AudioChannelSet::leftSurround:      return kSpeakerLs;
        case AudioChannelSet::rightSurround:     return kSpeakerRs;
        case AudioChannelSet::leftCentre:        return kSpeakerLc;
        case AudioChannelSet::rightCentre:       return kSpeakerRc;
        case AudioChannelSet::surround:          return kSpeakerS;
        case AudioChannelSet::leftRearSurround:  return kSpeakerSl;
        case AudioChannelSet::rightRearSurround: return kSpeakerSr;
        case AudioChannelSet::topMiddle:         return kSpeakerTm;
        case AudioChannelSet::topFrontLeft:      return kSpeakerTfl;
        case AudioChannelSet::topFrontCentre:    return kSpeakerTfc;
        case AudioChannelSet::topFrontRight:     return kSpeakerTfr;
        case AudioChannelSet::topRearLeft:       return kSpeakerTrl;
        case AudioChannelSet::topRearCentre:     return kSpeakerTrc;
        case AudioChannelSet::topRearRight:      return kSpeakerTrr;
        case AudioChannelSet::subbass2:          return kSpeakerLfe2;
        default: break;
    }

    return 0;
}

static inline AudioChannelSet::ChannelType getChannelType (Steinberg::Vst::Speaker type) noexcept
{
    using namespace Steinberg::Vst;

    switch (type)
    {
        case kSpeakerL:     return AudioChannelSet::left;
        case kSpeakerR:     return AudioChannelSet::right;
        case kSpeakerC:     return AudioChannelSet::centre;
        case kSpeakerLfe:   return AudioChannelSet::subbass;
        case kSpeakerLs:    return AudioChannelSet::leftSurround;
        case kSpeakerRs:    return AudioChannelSet::rightSurround;
        case kSpeakerLc:    return AudioChannelSet::leftCentre;
        case kSpeakerRc:    return AudioChannelSet::rightCentre;
        case kSpeakerS:     return AudioChannelSet::surround;
        case kSpeakerSl:    return AudioChannelSet::leftRearSurround;
        case kSpeakerSr:    return AudioChannelSet::rightRearSurround;
        case kSpeakerTm:    return AudioChannelSet::topMiddle;
        case kSpeakerTfl:   return AudioChannelSet::topFrontLeft;
        case kSpeakerTfc:   return AudioChannelSet::topFrontCentre;
        case kSpeakerTfr:   return AudioChannelSet::topFrontRight;
        case kSpeakerTrl:   return AudioChannelSet::topRearLeft;
        case kSpeakerTrc:   return AudioChannelSet::topRearCentre;
        case kSpeakerTrr:   return AudioChannelSet::topRearRight;
        case kSpeakerLfe2:  return AudioChannelSet::subbass2;
        default: break;
    }

    return AudioChannelSet::unknown;
}

static inline Steinberg::Vst::SpeakerArrangement getSpeakerArrangement (const AudioChannelSet& channels) noexcept
{
     // treat mono as special case as we do not have a designated mono speaker
    if (channels == AudioChannelSet::mono())
        return Steinberg::Vst::kSpeakerM;

    Steinberg::Vst::SpeakerArrangement result = 0;

    Array<AudioChannelSet::ChannelType> types (channels.getChannelTypes());

    for (int i = 0; i < types.size(); ++i)
        result |= getSpeakerType (types.getReference(i));

    return result;
}

static inline AudioChannelSet getChannelSetForSpeakerArrangement (Steinberg::Vst::SpeakerArrangement arr) noexcept
{
    // treat mono as special case as we do not have a designated mono speaker
    if (arr == Steinberg::Vst::kSpeakerM)
        return AudioChannelSet::mono();

    AudioChannelSet result;

    for (Steinberg::Vst::Speaker speaker = 1; speaker <= Steinberg::Vst::kSpeakerRcs; speaker <<= 1)
        if ((arr & speaker) != 0)
            result.addChannel (getChannelType (speaker));

    return result;
}

//==============================================================================
template <class ObjectType>
class ComSmartPtr
{
public:
    ComSmartPtr() noexcept : source (nullptr) {}
    ComSmartPtr (ObjectType* object, bool autoAddRef = true) noexcept  : source (object)    { if (source != nullptr && autoAddRef) source->addRef(); }
    ComSmartPtr (const ComSmartPtr& other) noexcept : source (other.source)                 { if (source != nullptr) source->addRef(); }
    ~ComSmartPtr()                                                                          { if (source != nullptr) source->release(); }

    operator ObjectType*() const noexcept    { return source; }
    ObjectType* get() const noexcept         { return source; }
    ObjectType& operator*() const noexcept   { return *source; }
    ObjectType* operator->() const noexcept  { return source; }

    ComSmartPtr& operator= (const ComSmartPtr& other)       { return operator= (other.source); }

    ComSmartPtr& operator= (ObjectType* const newObjectToTakePossessionOf)
    {
        ComSmartPtr p (newObjectToTakePossessionOf);
        std::swap (p.source, source);
        return *this;
    }

    bool operator== (ObjectType* const other) noexcept { return source == other; }
    bool operator!= (ObjectType* const other) noexcept { return source != other; }

    bool loadFrom (Steinberg::FUnknown* o)
    {
        *this = nullptr;
        return o != nullptr && o->queryInterface (ObjectType::iid, (void**) &source) == Steinberg::kResultOk;
    }

    bool loadFrom (Steinberg::IPluginFactory* factory, const Steinberg::TUID& uuid)
    {
        jassert (factory != nullptr);
        *this = nullptr;
        return factory->createInstance (uuid, ObjectType::iid, (void**) &source) == Steinberg::kResultOk;
    }

private:
    ObjectType* source;
};

//==============================================================================
class MidiEventList  : public Steinberg::Vst::IEventList
{
public:
    MidiEventList() {}
    virtual ~MidiEventList() {}

    JUCE_DECLARE_VST3_COM_REF_METHODS
    JUCE_DECLARE_VST3_COM_QUERY_METHODS

    //==============================================================================
    void clear()
    {
        events.clearQuick();
    }

    Steinberg::int32 PLUGIN_API getEventCount() override
    {
        return (Steinberg::int32) events.size();
    }

    // NB: This has to cope with out-of-range indexes from some plugins.
    Steinberg::tresult PLUGIN_API getEvent (Steinberg::int32 index, Steinberg::Vst::Event& e) override
    {
        if (isPositiveAndBelow ((int) index, events.size()))
        {
            e = events.getReference ((int) index);
            return Steinberg::kResultTrue;
        }

        return Steinberg::kResultFalse;
    }

    Steinberg::tresult PLUGIN_API addEvent (Steinberg::Vst::Event& e) override
    {
        events.add (e);
        return Steinberg::kResultTrue;
    }

    //==============================================================================
    static void toMidiBuffer (MidiBuffer& result, Steinberg::Vst::IEventList& eventList)
    {
        const int32 numEvents = eventList.getEventCount();

        for (Steinberg::int32 i = 0; i < numEvents; ++i)
        {
            Steinberg::Vst::Event e;

            if (eventList.getEvent (i, e) == Steinberg::kResultOk)
            {
                switch (e.type)
                {
                    case Steinberg::Vst::Event::kNoteOnEvent:
                        result.addEvent (MidiMessage::noteOn (createSafeChannel (e.noteOn.channel),
                                                              createSafeNote (e.noteOn.pitch),
                                                              (Steinberg::uint8) denormaliseToMidiValue (e.noteOn.velocity)),
                                         e.sampleOffset);
                        break;

                    case Steinberg::Vst::Event::kNoteOffEvent:
                        result.addEvent (MidiMessage::noteOff (createSafeChannel (e.noteOff.channel),
                                                               createSafeNote (e.noteOff.pitch),
                                                               (Steinberg::uint8) denormaliseToMidiValue (e.noteOff.velocity)),
                                         e.sampleOffset);
                        break;

                    case Steinberg::Vst::Event::kPolyPressureEvent:
                        result.addEvent (MidiMessage::aftertouchChange (createSafeChannel (e.polyPressure.channel),
                                                                        createSafeNote (e.polyPressure.pitch),
                                                                        denormaliseToMidiValue (e.polyPressure.pressure)),
                                         e.sampleOffset);
                        break;

                    case Steinberg::Vst::Event::kDataEvent:
                        result.addEvent (MidiMessage::createSysExMessage (e.data.bytes, (int) e.data.size),
                                         e.sampleOffset);
                        break;

                    default:
                        break;
                }
            }
        }
    }

    static void toEventList (Steinberg::Vst::IEventList& result, MidiBuffer& midiBuffer)
    {
        MidiBuffer::Iterator iterator (midiBuffer);
        MidiMessage msg;
        int midiEventPosition = 0;

        enum { maxNumEvents = 2048 }; // Steinberg's Host Checker states that no more than 2048 events are allowed at once
        int numEvents = 0;

        while (iterator.getNextEvent (msg, midiEventPosition))
        {
            if (++numEvents > maxNumEvents)
                break;

            Steinberg::Vst::Event e = { 0 };

            if (msg.isNoteOn())
            {
                e.type              = Steinberg::Vst::Event::kNoteOnEvent;
                e.noteOn.channel    = createSafeChannel (msg.getChannel());
                e.noteOn.pitch      = createSafeNote (msg.getNoteNumber());
                e.noteOn.velocity   = normaliseMidiValue (msg.getVelocity());
                e.noteOn.length     = 0;
                e.noteOn.tuning     = 0.0f;
                e.noteOn.noteId     = -1;
            }
            else if (msg.isNoteOff())
            {
                e.type              = Steinberg::Vst::Event::kNoteOffEvent;
                e.noteOff.channel   = createSafeChannel (msg.getChannel());
                e.noteOff.pitch     = createSafeNote (msg.getNoteNumber());
                e.noteOff.velocity  = normaliseMidiValue (msg.getVelocity());
                e.noteOff.tuning    = 0.0f;
                e.noteOff.noteId    = -1;
            }
            else if (msg.isSysEx())
            {
                e.type          = Steinberg::Vst::Event::kDataEvent;
                e.data.bytes    = msg.getSysExData();
                e.data.size     = (uint32) msg.getSysExDataSize();
                e.data.type     = Steinberg::Vst::DataEvent::kMidiSysEx;
            }
            else if (msg.isAftertouch())
            {
                e.type                   = Steinberg::Vst::Event::kPolyPressureEvent;
                e.polyPressure.channel   = createSafeChannel (msg.getChannel());
                e.polyPressure.pitch     = createSafeNote (msg.getNoteNumber());
                e.polyPressure.pressure  = normaliseMidiValue (msg.getAfterTouchValue());
            }
            else
            {
                continue;
            }

            e.busIndex = 0;
            e.sampleOffset = midiEventPosition;

            result.addEvent (e);
        }
    }

private:
    Array<Steinberg::Vst::Event, CriticalSection> events;
    Atomic<int> refCount;

    static Steinberg::int16 createSafeChannel (int channel) noexcept  { return (Steinberg::int16) jlimit (0, 15, channel - 1); }
    static int createSafeChannel (Steinberg::int16 channel) noexcept  { return (int) jlimit (1, 16, channel + 1); }

    static Steinberg::int16 createSafeNote (int note) noexcept        { return (Steinberg::int16) jlimit (0, 127, note); }
    static int createSafeNote (Steinberg::int16 note) noexcept        { return jlimit (0, 127, (int) note); }

    static float normaliseMidiValue (int value) noexcept              { return jlimit (0.0f, 1.0f, (float) value / 127.0f); }
    static int denormaliseToMidiValue (float value) noexcept          { return roundToInt (jlimit (0.0f, 127.0f, value * 127.0f)); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiEventList)
};

//==============================================================================
template <typename FloatType>
struct VST3BufferExchange
{
    typedef Array<FloatType*> Bus;
    typedef Array<Bus> BusMap;

    static inline void assignRawPointer (Steinberg::Vst::AudioBusBuffers& vstBuffers, float** raw)  { vstBuffers.channelBuffers32 = raw; }
    static inline void assignRawPointer (Steinberg::Vst::AudioBusBuffers& vstBuffers, double** raw) { vstBuffers.channelBuffers64 = raw; }

    /** Assigns a series of AudioSampleBuffer's channels to an AudioBusBuffers'

        @warning For speed, does not check the channel count and offsets
                 according to the AudioSampleBuffer
    */
    static void associateBufferTo (Steinberg::Vst::AudioBusBuffers& vstBuffers,
                                   Bus& bus,
                                   AudioBuffer<FloatType>& buffer,
                                   int numChannels, int channelStartOffset,
                                   int sampleOffset = 0)
    {
        const int channelEnd = numChannels + channelStartOffset;
        jassert (channelEnd >= 0 && channelEnd <= buffer.getNumChannels());

        bus.clearQuick();

        for (int i = channelStartOffset; i < channelEnd; ++i)
            bus.add (buffer.getWritePointer (i, sampleOffset));

        assignRawPointer (vstBuffers, bus.getRawDataPointer());
        vstBuffers.numChannels      = numChannels;
        vstBuffers.silenceFlags     = 0;
    }

    static void mapArrangementToBusses (int& channelIndexOffset, int index,
                                        Array<Steinberg::Vst::AudioBusBuffers>& result,
                                        BusMap& busMapToUse, Steinberg::Vst::SpeakerArrangement arrangement,
                                        AudioBuffer<FloatType>& source)
    {
        const int numChansForBus = BigInteger ((juce::int64) arrangement).countNumberOfSetBits();

        if (index >= result.size())
            result.add (Steinberg::Vst::AudioBusBuffers());

        if (index >= busMapToUse.size())
            busMapToUse.add (Bus());

        if (numChansForBus > 0)
            associateBufferTo (result.getReference (index),
                               busMapToUse.getReference (index),
                               source, numChansForBus, channelIndexOffset);

        channelIndexOffset += numChansForBus;
    }

    static inline void mapBufferToBusses (Array<Steinberg::Vst::AudioBusBuffers>& result, BusMap& busMapToUse,
                                          const Array<Steinberg::Vst::SpeakerArrangement>& arrangements,
                                          AudioBuffer<FloatType>& source)
    {
        int channelIndexOffset = 0;

        for (int i = 0; i < arrangements.size(); ++i)
            mapArrangementToBusses (channelIndexOffset, i, result, busMapToUse,
                                    arrangements.getUnchecked (i), source);
    }

    static inline void mapBufferToBusses (Array<Steinberg::Vst::AudioBusBuffers>& result,
                                          Steinberg::Vst::IAudioProcessor& processor,
                                          BusMap& busMapToUse, bool isInput, int numBusses,
                                          AudioBuffer<FloatType>& source)
    {
        int channelIndexOffset = 0;

        for (int i = 0; i < numBusses; ++i)
            mapArrangementToBusses (channelIndexOffset, i,
                                    result, busMapToUse,
                                    getArrangementForBus (&processor, isInput, i),
                                    source);
    }
};

template <typename FloatType>
struct VST3FloatAndDoubleBusMapCompositeHelper {};

struct VST3FloatAndDoubleBusMapComposite
{
    VST3BufferExchange<float>::BusMap  floatVersion;
    VST3BufferExchange<double>::BusMap doubleVersion;

    template <typename FloatType>
    inline typename VST3BufferExchange<FloatType>::BusMap& get()   { return VST3FloatAndDoubleBusMapCompositeHelper<FloatType>::get (*this); }
};


template <> struct VST3FloatAndDoubleBusMapCompositeHelper<float>
{
    static inline VST3BufferExchange<float>::BusMap& get (VST3FloatAndDoubleBusMapComposite& impl)  { return impl.floatVersion; }
};

template <> struct VST3FloatAndDoubleBusMapCompositeHelper<double>
{
    static inline VST3BufferExchange<double>::BusMap& get (VST3FloatAndDoubleBusMapComposite& impl) { return impl.doubleVersion; }
};
#endif   // JUCE_VST3COMMON_H_INCLUDED
