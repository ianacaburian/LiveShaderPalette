/*
  ==============================================================================

    LiveShaderProgram.h
    Created: 9 Feb 2019 9:20:32am
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "OpenGLComponent.h"
class LiveShaderPanel;
class MainComponent;

//==============================================================================

class LiveShaderProgram
{
public:
    //==========================================================================

    LiveShaderProgram(MainComponent& parent, LiveShaderPanel& panel, const File& fragment_file);
    ~LiveShaderProgram() = default;
    
    //==========================================================================

    void create();
    void render();
    void delete_program();

private:
    //==========================================================================

    static constexpr const char* const default_vertex_source =
        "in vec4 position;\nvoid main()\n{\ngl_Position=position;\n}\n";
    static constexpr const char* const default_fragment_source =
        "out vec4 out_color;\nvoid main()\n{\n"
        "float result=0.;\nif(-1.<=gl_PointCoord.y&&gl_PointCoord.y<-0.95\n"
         "||0.95<=gl_PointCoord.y&&gl_PointCoord.y<1.\n"
         "||-1.<=gl_PointCoord.x&&gl_PointCoord.x<-0.95\n"
         "||0.95<=gl_PointCoord.x&&gl_PointCoord.x<1.){\n"
         "result=1.;\n}\nout_color=vec4(result,result,result,1.);\n}\n";
    String fragment_shader_source{};
    std::unique_ptr<OpenGLShaderProgram> program;
    MainComponent& parent;
    LiveShaderPanel& panel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderProgram)
};
