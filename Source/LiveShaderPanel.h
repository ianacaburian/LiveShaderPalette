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
#include "OpenGLRectangle.h"
class MainComponent;
class LiveShaderProgram;
//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */

class LiveShaderPanel   : public OpenGLChildComponent
{
public:
    //==============================================================================
    LiveShaderPanel(MainComponent& parent, const int component_ID);
    ~LiveShaderPanel();
    
    //==============================================================================
    void mouseDown(const MouseEvent& mouse_event) override;
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    //==============================================================================
    void recompile_shader();
    float get_rendering_scale() const;
    float get_sin_time() const;
    int get_panel_ID() const;

private:
    //==============================================================================
    MainComponent& parent;
    Serialization serialization;
    OpenGLRectangle rectangle;
    std::unique_ptr<LiveShaderProgram> live_shader_program;
    const int panel_ID;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderPanel)
};
