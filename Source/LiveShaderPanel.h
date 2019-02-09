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
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */
using GL = juce::OpenGLExtensionFunctions;
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
    float get_desktop_scale() const;
    float get_sin_time() const;
    int get_ID() const;

private:
    //==============================================================================
    static constexpr int positions_count = 8;
    GLfloat positions[positions_count] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f,
        -1.0f,  1.0f
    };
    static constexpr int elements_count = 6;
    GLuint elements[elements_count] = {
        0, 1, 2,
        0, 2, 3
    };
    MainComponent& parent;
    Serialization serialization;
    std::unique_ptr<LiveShaderProgram> live_shader_program;
    GLuint vertex_arr_ID{}, vertex_buff_ID{}, index_buff_ID{};
    
    // "Position" vertex attribute parameters
    static constexpr int pos_attrib_id = 0;
    static constexpr int num_floats_per_pos_attrib = 2;
    const int component_ID;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderPanel)
};
