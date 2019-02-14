/*
  ==============================================================================

    LiveShaderProgram.cpp
    Created: 9 Feb 2019 11:10:32am
    Author:  Ian Caburian

  ==============================================================================
*/

#include "LiveShaderProgram.h"
#include "LiveShaderPanel.h"
#include "MainComponent.h"

//==============================================================================

LiveShaderProgram::LiveShaderProgram(MainComponent& parent, LiveShaderPanel& panel, const File& fragment_file)
: fragment_shader_source{ fragment_file.loadFileAsString() }
, parent{ parent }
, panel{ panel }
{
}

//==============================================================================

void LiveShaderProgram::create()
{
    shader_prog_ID = GL::glCreateProgram();
    const auto default_vertex_source = LiveShader::create_default_shader_source(GL_VERTEX_SHADER);
    auto vertex_shader =    LiveShader{ GL_VERTEX_SHADER, default_vertex_source.getCharPointer(),
                                        static_cast<GLint>(sizeof(GLchar) * default_vertex_source.length()),
                                        shader_prog_ID };
    auto fragment_shader =  LiveShader{ GL_FRAGMENT_SHADER, fragment_shader_source.getCharPointer(),
                                        static_cast<GLint>(sizeof(GLchar) * fragment_shader_source.length()),
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
        
        const auto error_log_msg = String{ "s " } + Time::getCurrentTime().formatted("%H:%M:%S") + " " + result.getErrorMessage();
        MessageManager::callAsync([error_log_msg]{
            Logger::writeToLog(error_log_msg);
        });
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

LiveShaderProgram::Uniforms::Uniforms(LiveShaderProgram& program)
: program{ program }
{
}
void LiveShaderProgram::Uniforms::create()
{
    const auto layout = program.parent.get_layout();
    GL::glUniform4i(get_uniform_location("uf_componentID_layout"),
                    program.panel.getComponentID().getIntValue(),
                    layout.first, layout.second, 0);
    
    uf_screen_size          = get_uniform_location("uf_panel_size");
    uf_rendering_scale      = get_uniform_location("uf_rendering_scale");
    uf_mouse_type           = get_uniform_location("uf_mouse_type");
    uf_mouse_position       = get_uniform_location("uf_mouse_position");
    uf_time                 = get_uniform_location("uf_time");
    uf_flags                = get_uniform_location("uf_flags");
    uf_mouse_options        = get_uniform_location("uf_mouse_options");

}
void LiveShaderProgram::Uniforms::send_uniforms()
{
    const auto panel_area_size = program.parent.get_panel_area_size();
    const auto mouse_state = program.panel.copyMouseState();
    const auto mouse_options = mouse_options_to_float(mouse_state.lastEventType);
    
    GL::glUniform4i(uf_screen_size,         program.panel.getWidth(), program.panel.getHeight(),
                                            panel_area_size.x, panel_area_size.y);
    GL::glUniform1f(uf_rendering_scale,     program.parent.getRenderingScale());
    GL::glUniform1i(uf_mouse_type,          static_cast<int>(mouse_state.lastEventType.index()));
    GL::glUniform4f(uf_mouse_position,      mouse_state.mousePosition.x, mouse_state.mousePosition.y,
                                            mouse_state.mouseDownPosition.x, mouse_state.mouseDownPosition.y);
    GL::glUniform4f(uf_time,                mouse_state.eventTime, mouse_state.mouseDownTime,
                                            program.parent.get_sin_time(), program.parent.get_saw_time());
    GL::glUniform4i(uf_flags,               mouse_state.isDown, mouse_state.isToggled, mouse_state.isRightClick, 0);
    GL::glUniform2f(uf_mouse_options,       mouse_options.x, mouse_options.y);
}
GLint LiveShaderProgram::Uniforms::get_uniform_location(const char* uniform_id) const
{
    return GL::glGetUniformLocation(program.shader_prog_ID, uniform_id);
}
Point<float> LiveShaderProgram::Uniforms::mouse_options_to_float(const MouseVariant& last_event_type)
{
    return std::visit([&](auto&& m)->Point<float> {
        using T = std::decay_t<decltype(m)>;
        if constexpr (std::is_same_v<T, MouseType::WheelMove>) {
            return { m.wheel.deltaX, m.wheel.deltaY };
        }
        else if constexpr (std::is_same_v<T, MouseType::Magnify>) {
            return { m.scaleFactor, 0.f };
        }
        return { 0.f, 0.f };
    }, last_event_type);
}
//==============================================================================

Result LiveShaderProgram::verify_operation_sucess(GLuint object_id, const GLenum type)
{
    GLint success;
    (type == GL_COMPILE_STATUS ? GL::glGetShaderiv : GL::glGetProgramiv)
        (object_id, type, &success);
    if (! success) {    // TODO: create and print to a separate console window
        GLint length;
        glGetShaderiv(object_id, GL_INFO_LOG_LENGTH, &length);
        (type == GL_COMPILE_STATUS ? GL::glGetShaderiv : GL::glGetProgramiv)
            (object_id, GL_INFO_LOG_LENGTH, &length);
        auto* message = static_cast<char*>(alloca(sizeof(char) * length));
        (type == GL_COMPILE_STATUS ? GL::glGetShaderInfoLog : GL::glGetProgramInfoLog)
            (object_id, length, &length, message);
        return Result::fail(message);
    }
    return Result::ok();
}
