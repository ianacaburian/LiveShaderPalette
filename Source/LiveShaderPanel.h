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
class MainComponent;
class LiveShaderProgram;

//==============================================================================

class LiveShaderPanel   : public OpenGLChildComponent
{
public:
    //==========================================================================

    explicit LiveShaderPanel(MainComponent& parent, const int component_ID);
    ~LiveShaderPanel() = default;
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;
    
    //==========================================================================
    
    void load_shader_file(const File& file);
    void recompile_shader();
    String get_current_fragment_file_path() const;
    
private:
    //==========================================================================
    
    OpenGLRectangle rectangle;
    File fragment_file{};
    std::unique_ptr<LiveShaderProgram> live_shader_program;
    MainComponent& parent;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderPanel)
};
