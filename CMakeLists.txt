cmake_minimum_required(VERSION 3.22)

project(Alturadsp VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(JUCE)

juce_add_plugin(Alturadsp
    COMPANY_NAME "AlturaDSP"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Altr
    PLUGIN_CODE Altr
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Alturadsp")

juce_generate_juce_header(Alturadsp)

target_sources(Alturadsp
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/AmpModeling.cpp
        Source/CabSimulation.cpp
        Source/EffectsChain.cpp
        Source/PresetManager.cpp
        Source/DSPUtils.cpp
        Source/AmpSection.cpp
        Source/CabSection.cpp
        Source/EffectsSection.cpp
        Source/PresetSection.cpp)

target_compile_definitions(Alturadsp
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Alturadsp
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
