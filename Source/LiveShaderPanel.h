/*
  ==============================================================================

    LiveShaderPanel.h
    Created: 8 Feb 2019 4:51:36am
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "OpenGLComponent.h"
#include "Serialization.h"
class MainComponent;
class LiveShaderProgram;

//==============================================================================

class LiveShaderPanel   : public OpenGLChildComponent
{
public:
    //==============================================================================

    struct MouseState
    {
        MouseVariant mouse_variant{ MouseType::Up{} };
        Point<float> mouse_position{}, mouse_down_position{}, wheel_delta{};
        float event_time{}, mouse_down_time{}, scale_factor{};
        bool is_down{}, is_right_click{}, is_toggled{};
        
        void capture_event(const MouseEvent& mouse_event);
    };

    //==============================================================================

    explicit LiveShaderPanel(MainComponent& parent, const int component_ID);
    ~LiveShaderPanel();
    void mouseDown(const MouseEvent& mouse_event) override;
    void mouseUp(const MouseEvent& mouse_event) override;
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    //==============================================================================
    
    void load_shader_file(const String& file_path);
    void recompile_shader();
    MouseState get_mouse_state() const;
    float get_rendering_scale() const;
    float get_sin_time() const;
    int get_panel_ID() const;

private:
    //==============================================================================

    OpenGLRectangle rectangle;
    Serialization serialization{ "Vert.vert", "Frag.frag" };
    MouseState mouse_state;
    
    std::unique_ptr<LiveShaderProgram> live_shader_program;
    MainComponent& parent;
    const int panel_ID;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderPanel)
};
