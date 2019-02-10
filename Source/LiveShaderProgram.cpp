/*
  ==============================================================================

    LiveShaderProgram.cpp
    Created: 9 Feb 2019 11:10:32am
    Author:  Ian Caburian

  ==============================================================================
*/

#include "LiveShaderProgram.h"
#include "LiveShaderPanel.h"

//==============================================================================

LiveShaderProgram::LiveShaderProgram(LiveShaderPanel& panel, const File& vertex_file, const File& fragment_file)
: panel{ panel }
, shader_program_source{ vertex_file.loadFileAsString(), fragment_file.loadFileAsString() }
{
}

//==============================================================================

void LiveShaderProgram::create()
{
    shader_prog_ID = GL::glCreateProgram();
    auto vertex_shader =    LiveShader{ GL_VERTEX_SHADER, shader_program_source.vertex.getCharPointer(),
                                        static_cast<GLint>(sizeof(GLchar) * shader_program_source.vertex.length()),
                                        shader_prog_ID };
    auto fragment_shader =  LiveShader{ GL_FRAGMENT_SHADER, shader_program_source.fragment.getCharPointer(),
                                        static_cast<GLint>(sizeof(GLchar) * shader_program_source.fragment.length()),
                                        shader_prog_ID };
    GL::glLinkProgram(shader_prog_ID);
    
    GLint success;
    GL::glGetProgramiv(shader_prog_ID, GL_LINK_STATUS, &success);
    if (!success) {
        char info[max_info_chars];
        GL::glGetProgramInfoLog(shader_prog_ID, max_info_chars, nullptr, info);
        DBG(info);
        jassertfalse;
    }
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

//==============================================================================

LiveShaderProgram::LiveShader::LiveShader(const GLenum type, const GLchar* source, const GLint source_length, const GLuint shader_prog_ID)
{
    auto create_shader = [this, type](const GLchar* source, const GLint source_length) {
        shader_ID = GL::glCreateShader(type);
        GL::glShaderSource(shader_ID, 1, &source, &source_length);
        GL::glCompileShader(shader_ID);
    };
    create_shader(source, source_length);

    GLint success;
    GL::glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &success);
    if (! success) {    // TODO: create and print to a separate console window
        char info[max_info_chars];
        GL::glGetShaderInfoLog(shader_ID, max_info_chars, nullptr, info);
        DBG(Time::getCurrentTime().formatted("%I:%M:%S") << " " << info);
        const auto default_shader_source = create_default_shader_source(type);
        create_shader(default_shader_source.getCharPointer(), sizeof(GLchar) * default_shader_source.length());
    }
    GL::glAttachShader(shader_prog_ID, shader_ID);
}
LiveShaderProgram::LiveShader::~LiveShader()
{
    GL::glDeleteShader(shader_ID);
}
String LiveShaderProgram::LiveShader::create_default_shader_source(const GLenum type)
{
    String s = "#version 150\n";
    s << (type == GL_VERTEX_SHADER ? "in vec4 position" : "out vec4 color")
      << ";\nvoid main()\n{\n"
      << (type == GL_VERTEX_SHADER ? "gl_Position = position" : "color = vec4(.0, .0, .5, 1.)")
      << ";\n}\n";
    return s;
}

//==============================================================================

void LiveShaderProgram::create_uniforms()
{
    GL::glUniform4f(get_uniform_location("uf_panel"), panel.getWidth(), panel.getHeight(), panel.get_rendering_scale(), panel.get_panel_ID());
    uf_sin_time = get_uniform_location("uf_sin_time");
}
GLint LiveShaderProgram::get_uniform_location(const char* uniform_id) const { return GL::glGetUniformLocation(shader_prog_ID, uniform_id); }
