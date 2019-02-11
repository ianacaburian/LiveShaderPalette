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

LiveShaderProgram::LiveShaderProgram(LiveShaderPanel& panel,
                                     const File& vertex_file, const File& fragment_file)
: shader_program_source{ vertex_file.loadFileAsString(), fragment_file.loadFileAsString() }
, panel{ panel }
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
    
    if (const auto result = verify_operation_sucess(shader_prog_ID, GL_LINK_STATUS);
        result.failed()) {

        DBG(result.getErrorMessage());
        jassertfalse;
    }
    use();
    uniforms.create();
}
void LiveShaderProgram::use() { GL::glUseProgram(shader_prog_ID); }
void LiveShaderProgram::render()
{
    use();
    uniforms.send_uniforms();
}
void LiveShaderProgram::delete_program() { GL::glDeleteProgram(shader_prog_ID); }

//==============================================================================

LiveShaderProgram::LiveShader::LiveShader(const GLenum type, const GLchar* source,
                                          const GLint source_length, const GLuint shader_prog_ID)
: shader_ID{ GL::glCreateShader(type) }
{
    auto create_shader = [this, type](const GLchar* source, const GLint source_length) {
        GL::glShaderSource(shader_ID, 1, &source, &source_length);
        GL::glCompileShader(shader_ID);
    };
    create_shader(source, source_length);

    if (const auto result = LiveShaderProgram::verify_operation_sucess(shader_ID, GL_COMPILE_STATUS);
        result.failed()) {
        
        DBG(Time::getCurrentTime().formatted("%I:%M:%S") << " " << result.getErrorMessage());
        const auto default_shader_source = create_default_shader_source(type);
        create_shader(default_shader_source.getCharPointer(),
                      sizeof(GLchar) * default_shader_source.length());
    }
    GL::glAttachShader(shader_prog_ID, shader_ID);
}
LiveShaderProgram::LiveShader::~LiveShader()
{
    GL::glDeleteShader(shader_ID);
}
String LiveShaderProgram::LiveShader::create_default_shader_source(const GLenum type)
{
    auto s = String{ "#version 150\n" };
    s << (type == GL_VERTEX_SHADER ? "in vec4 position" : "out vec4 color")
      << ";\nvoid main()\n{\n"
      << (type == GL_VERTEX_SHADER ? "gl_Position = position" : "color = vec4(.0, .0, .5, 1.)")
      << ";\n}\n";
    return s;
}

//==============================================================================

LiveShaderProgram::Uniforms::Uniforms(LiveShaderProgram& parent)
: parent{ parent }
{
}
void LiveShaderProgram::Uniforms::create()
{
    // Static uniforms
    GL::glUniform4f(get_uniform_location("uf_panel"), parent.panel.getWidth(), parent.panel.getHeight(),
                    parent.panel.get_rendering_scale(), parent.panel.getComponentID().getFloatValue());
    
    // Dynamic uniforms
    uf_sin_time             = get_uniform_location("uf_sin_time");
    uf_mouse_type           = get_uniform_location("uf_mouse_type");
    uf_mouse_position       = get_uniform_location("uf_mouse_position");
    
}
void LiveShaderProgram::Uniforms::send_uniforms()
{
    GL::glUniform1f(uf_sin_time, parent.panel.get_sin_time());
    
    const auto mouse_state = parent.panel.copyMouseState();
    GL::glUniform2f(uf_mouse_position, mouse_state.mousePosition.x, mouse_state.mousePosition.y);
}
GLint LiveShaderProgram::Uniforms::get_uniform_location(const char* uniform_id) const
{
    return GL::glGetUniformLocation(parent.shader_prog_ID, uniform_id);
}

//==============================================================================

Result LiveShaderProgram::verify_operation_sucess(GLuint object_id, const GLenum type)
{
    GLint success;
    (type == GL_COMPILE_STATUS ? GL::glGetShaderiv : GL::glGetProgramiv)(object_id, type, &success);
    if (! success) {    // TODO: create and print to a separate console window
        GLint length;
        glGetShaderiv(object_id, GL_INFO_LOG_LENGTH, &length);
        (type == GL_COMPILE_STATUS ? GL::glGetShaderiv : GL::glGetProgramiv)(object_id, GL_INFO_LOG_LENGTH, &length);
        auto* message = static_cast<char*>(alloca(sizeof(char) * length));
        (type == GL_COMPILE_STATUS ? GL::glGetShaderInfoLog : GL::glGetProgramInfoLog)(object_id, length, &length, message);
        return Result::fail(message);
    }
    return Result::ok();
}
