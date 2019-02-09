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
class LiveShaderProgram
{
    using GL = juce::OpenGLExtensionFunctions;
public:
    struct ShaderProgramSource
    {
        String vertex;
        String fragment;
    };
    LiveShaderProgram(LiveShaderPanel& panel, const File& vertex_file, const File& fragment_file);
    ~LiveShaderProgram() = default;
    void create();
    void use();
    void render();
    void delete_program();
private:
    LiveShaderPanel& panel;
    ShaderProgramSource shader_program_source{};
    GLuint shader_prog_ID{};
    GLint uf_sin_time{};
    
    static GLuint create_shader(const GLenum type, const GLchar* source, const GLint source_length, const bool check = true);
    void create_uniforms();
    GLint get_uniform_location(const char* uniform_id) const;
};
