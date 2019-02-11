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
    using GL = juce::OpenGLExtensionFunctions;
public:
    //==============================================================================

    LiveShaderProgram(MainComponent& parent, LiveShaderPanel& panel, const File& vertex_file, const File& fragment_file);
    ~LiveShaderProgram() = default;
    
    //==============================================================================

    void create();
    void use();
    void render();
    void delete_program();

private:
    //==============================================================================

    struct LiveShader
    {
        GLuint shader_ID;
        
        LiveShader(const GLenum type, const GLchar* source, const GLint source_length, const GLuint shader_prog_ID);
        ~LiveShader();
        static String create_default_shader_source(const GLenum type);
    };
    
    //==============================================================================

    struct Uniforms
    {
        GLint
        uf_componentID_layout,
        uf_screen_size,
        uf_rendering_scale,
        uf_mouse_type,
        uf_mouse_position,
        uf_time,
        uf_flags,
        uf_mouse_options;
        LiveShaderProgram& program;
        
        Uniforms(LiveShaderProgram& program);
        void create();
        void send_uniforms();
        GLint get_uniform_location(const char* uniform_id) const;
        static Point<float> mouse_options_to_float(const MouseVariant& last_event_type);
    };
    
    //==============================================================================

    struct ShaderProgramSource
    {
        String vertex;
        String fragment;
    };

    //==============================================================================

    Uniforms uniforms{ *this };
    ShaderProgramSource shader_program_source{};
    GLuint shader_prog_ID{};
    MainComponent& parent;
    LiveShaderPanel& panel;
    
    //==============================================================================

    Point<int> get_screen_size() const;
    static Result verify_operation_sucess(GLuint object_id, const GLenum type);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderProgram)
};
