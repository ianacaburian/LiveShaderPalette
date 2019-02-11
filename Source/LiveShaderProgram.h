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

//==============================================================================

class LiveShaderProgram
{
    using GL = juce::OpenGLExtensionFunctions;
public:
    //==============================================================================

    LiveShaderProgram(LiveShaderPanel& panel, const File& vertex_file, const File& fragment_file);
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
        uf_sin_time,
        uf_mouse_type,
        uf_mouse_position;
        LiveShaderProgram& parent;
        
        Uniforms(LiveShaderProgram& parent);
        void create();
        void send_uniforms();
        GLint get_uniform_location(const char* uniform_id) const;
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
    LiveShaderPanel& panel;
    
    //==============================================================================

    static Result verify_operation_sucess(const GLenum type, GLuint object_id);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderProgram)
};
