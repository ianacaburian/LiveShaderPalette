/*
  ==============================================================================

    LiveShaderProgram.h
    Created: 9 Feb 2019 9:20:32am
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "LiveShaderPanel.h"
class LiveShaderProgram
{
public:
    struct ShaderProgramSource
    {
        String vertex;
        String fragment;
    };
    LiveShaderProgram(LiveShaderPanel& panel, const File& vertex_file, const File& fragment_file)
    : panel{ panel }
    , shader_program_source{ vertex_file.loadFileAsString(), fragment_file.loadFileAsString() }
    {
    }
    ~LiveShaderProgram() = default;
    void create()
    {
        const auto vxID = create_shader(GL_VERTEX_SHADER, shader_program_source.vertex.getCharPointer(),
                                        sizeof(GLchar) * shader_program_source.vertex.length());
        const auto fsID = create_shader(GL_FRAGMENT_SHADER, shader_program_source.fragment.getCharPointer(),
                                        sizeof(GLchar) * shader_program_source.fragment.length());
        shader_prog_ID = GL::glCreateProgram();
        GL::glAttachShader(shader_prog_ID, vxID);
        GL::glAttachShader(shader_prog_ID, fsID);
        GL::glLinkProgram(shader_prog_ID);
        
        GLint success;
        GL::glGetProgramiv(shader_prog_ID, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            GL::glGetProgramInfoLog(shader_prog_ID, 512, nullptr, infoLog);
            DBG(infoLog);
            jassertfalse;
        }
        GL::glDeleteShader(vxID);
        GL::glDeleteShader(fsID);
        use();
        
        // The resolution is static, computed at app initialization, and thus can afford to be sent just once here.
        const auto uf_panel = GL::glGetUniformLocation(shader_prog_ID, "uf_panel");
        GL::glUniform4f(uf_panel, panel.getWidth(), panel.getHeight(), panel.get_desktop_scale(), panel.get_ID());
        
        // The distance variable is dynamic, and thus is only declared here.
        // Sending it to the GPU is deferred as it must be computed and sent at each draw call.
        uf_sin_time = GL::glGetUniformLocation(shader_prog_ID, "uf_sin_time");
    }
    void use() { GL::glUseProgram(shader_prog_ID); }
    
    void render()
    {
        use();
        GL::glUniform1f(uf_sin_time, panel.get_sin_time());
    }
    void delete_program() { GL::glDeleteProgram(shader_prog_ID); }
private:
    LiveShaderPanel& panel;
    ShaderProgramSource shader_program_source{};
    GLuint shader_prog_ID{};
    GLint uf_sin_time{};
    
    static GLuint create_shader(const GLenum type, const GLchar* source, const GLint source_length, const bool check = true)
    {
        const auto shID = GL::glCreateShader(type);
        GL::glShaderSource(shID, 1, &source, &source_length);
        GL::glCompileShader(shID);
        
        GLint success;
        GL::glGetShaderiv(shID, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            GL::glGetShaderInfoLog(shID, 512, nullptr, infoLog);
            DBG(Time::getCurrentTime().formatted("%I:%M:%S") << " " << infoLog);
            String s = "#version 150\n";
            s << (type == GL_VERTEX_SHADER ? "in vec4 position" : "out vec4 color")
              << ";\nvoid main()\n{\n"
              << (type == GL_VERTEX_SHADER ? "gl_Position = position" : "color = vec4(.0, .0, .5, 1.)")
              << ";\n}\n";
            return create_shader(type, s.getCharPointer(), sizeof(GLchar) * s.length(), false);
        }
        return shID;
    }
};
