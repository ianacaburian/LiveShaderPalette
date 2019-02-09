/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "OpenGLComponent.h"
#include "OpenGLRectangle.h"

#include "Serialization.h"
#include "LiveShaderProgram.h"
class LiveShaderPanel;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public OpenGLParentComponent, private Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();
    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;
    void newOpenGLContextCreatedParent() override;
    void renderOpenGLParent() override;
    
    //==============================================================================
    float get_rendering_scale() const;
    float get_sin_time() const;
    
private:
    //==============================================================================
    std::vector<std::shared_ptr<LiveShaderPanel>> panels;
    Serialization serialization;
    OpenGLRectangle rectangle;
    std::unique_ptr<LiveShaderProgram> live_shader_program;
    Rectangle<int> button_bounds;
    TextButton
    live_compile    { "Live Compile" },
    print_sin_time    { "Print sin(time)" },
    print_ms_frame  { "Print ms/frame" };
    std::array<TextButton*, 3> buttons{
        &live_compile, &print_sin_time, &print_ms_frame
    };
    Point<int> screen_resolution { 400, 300 };
    double prev_time{}, ms_frame{};
    float sin_time{};
    int frame_count{}, compile_interval_ms = 2000;
    
    //==============================================================================

    void timerCallback() override;
    void init_buttons();
    void recompile_shaders();
    void print_log();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
