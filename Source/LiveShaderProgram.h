/*
  ==============================================================================

    LiveShaderProgram.h
    Created: 9 Feb 2019 9:20:32am
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class LiveShaderPanel;

//==============================================================================

class LiveShaderProgram
{
    using GL = juce::OpenGLExtensionFunctions;
public:
    //==============================================================================

    struct ShaderProgramSource
    {
        String vertex;
        String fragment;
    };
    
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
        LiveShader(const GLenum type, const GLchar* source, const GLint source_length, const GLuint shader_prog_ID);
        ~LiveShader();
        static String create_default_shader_source(const GLenum type);
        GLuint shader_ID;
    };
    
    //==============================================================================

    struct Uniforms
    {
        
    };
    
    //==============================================================================

    LiveShaderPanel& panel;
    ShaderProgramSource shader_program_source{};
    GLuint shader_prog_ID{};
    GLint uf_sin_time{};
    static constexpr int max_info_chars = 512;
    
    //==============================================================================

    void create_uniforms();
    GLint get_uniform_location(const char* uniform_id) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveShaderProgram)
};
