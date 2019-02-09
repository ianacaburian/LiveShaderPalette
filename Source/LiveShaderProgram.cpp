/*
  ==============================================================================

    LiveShaderProgram.cpp
    Created: 9 Feb 2019 11:10:32am
    Author:  Ian Caburian

  ==============================================================================
*/

#include "LiveShaderProgram.h"
#include "LiveShaderPanel.h"
LiveShaderProgram::LiveShaderProgram(LiveShaderPanel& panel, const File& vertex_file, const File& fragment_file)
: panel{ panel }
, shader_program_source{ vertex_file.loadFileAsString(), fragment_file.loadFileAsString() }
{
}
void LiveShaderProgram::create()
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
    create_uniforms();
}
void LiveShaderProgram::use() { GL::glUseProgram(shader_prog_ID); }
void LiveShaderProgram::render()
{
    use();
    GL::glUniform1f(uf_sin_time, panel.get_sin_time());
}
void LiveShaderProgram::delete_program() { GL::glDeleteProgram(shader_prog_ID); }
GLuint LiveShaderProgram::create_shader(const GLenum type, const GLchar* source, const GLint source_length, const bool check)
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
void LiveShaderProgram::create_uniforms()
{
    GL::glUniform4f(get_uniform_location("uf_panel"), panel.getWidth(), panel.getHeight(), panel.get_desktop_scale(), panel.get_panel_ID());
    
    uf_sin_time = get_uniform_location("uf_sin_time");
}
GLint LiveShaderProgram::get_uniform_location(const char* uniform_id) const { return GL::glGetUniformLocation(shader_prog_ID, uniform_id); }
