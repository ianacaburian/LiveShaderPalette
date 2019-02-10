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

    LiveShaderPanel(MainComponent& parent, const int component_ID);
    ~LiveShaderPanel();
    void mouseDown(const MouseEvent& mouse_event) override;
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    //==============================================================================
    
    void load_shader_file(const String& file_path);
    void recompile_shader();
    float get_rendering_scale() const;
    float get_sin_time() const;
    int get_panel_ID() const;

private:
    //==============================================================================

    MainComponent& parent;
    Serialization serialization{ "Vert.vert", "Frag.frag" };
    OpenGLRectangle rectangle;
    std::unique_ptr<LiveShaderProgram> live_shader_program;
    const int panel_ID;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderPanel)
};
