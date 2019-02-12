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

    explicit LiveShaderPanel(MainComponent& parent, const int component_ID);
    ~LiveShaderPanel();
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    //==============================================================================
    
    void load_shader_file(const String& file_path);
    void recompile_shader();
    void add_mouse_logger();
    void delete_mouse_logger();

private:
    //==============================================================================
    
    OpenGLRectangle rectangle;
    Serialization serialization{ "Vert.vert", "Frag.frag" };
    std::unique_ptr<LiveShaderProgram> live_shader_program;
    MainComponent& parent;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderPanel)
};
