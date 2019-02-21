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
    auto& gl = *OpenGLContext::getCurrentContext();
    program = std::make_unique<OpenGLShaderProgram>(gl);
    program->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(default_vertex_source));
    
    const auto fragment_ID = gl.extensions.glCreateShader(GL_FRAGMENT_SHADER);
    auto compile_shader = [&](const String& source) {
        const auto& versioned_source = (OpenGLShaderProgram::getLanguageVersion() > 1.2
                                        ? JUCE_GLSL_VERSION "\n"
                                        : "") + source;
        auto source_ptr = static_cast<const GLchar*>(versioned_source.getCharPointer());
        gl.extensions.glShaderSource(fragment_ID, 1, &source_ptr, nullptr);
        gl.extensions.glCompileShader(fragment_ID);
    };
    compile_shader(fragment_shader_source);

    DBG("fss: \n" << fragment_shader_source);
    GLint success = GL_FALSE;
    if (gl.extensions.glGetShaderiv(fragment_ID, GL_COMPILE_STATUS, &success);
        ! success || fragment_shader_source.isEmpty()) {
        
        parent.report_shader_compilation_error();
        
        const auto file_path = panel.get_current_fragment_file_path();
        const auto error_msg = [&] {
            if (fragment_shader_source.isEmpty()) {
                return String{ "Could not load source: " + file_path };
            }
            else {
                GLint length;
                gl.extensions.glGetShaderiv(fragment_ID, GL_INFO_LOG_LENGTH, &length);
                auto* message = static_cast<char*>(alloca(sizeof(char) * length));
                gl.extensions.glGetShaderInfoLog(fragment_ID, length, &length, message);
                return String{ message };
            }
        }();
        
        DBG("fss error: \n" << error_msg);
        MessageManager::callAsync([error_msg = error_msg.length() ? error_msg : "ERROR",
                                   timestamp = Time::getCurrentTime().formatted("%H:%M:%S"),
                                   component_ID = panel.getComponentID(),
                                   file_path] {
            auto s = String{ "s " };
            Logger::writeToLog(s << timestamp << " | componentID: " << component_ID <<
                               "\n" + file_path << "\n" + error_msg);
        });
        
        compile_shader(default_fragment_source);
    }
    gl.extensions.glAttachShader(program->getProgramID(), fragment_ID);
    gl.extensions.glDeleteShader(fragment_ID);
    
    program->link();
    program->use();
    const auto layout = parent.get_layout();
    program->setUniform("u_componentID_layout",
                        panel.getComponentID().getIntValue(),
                        layout.first, layout.second, 0);
}
void LiveShaderProgram::render()
{
    program->use();
    const auto shader_bounds = panel.getShaderBoundsToFloat();
    const auto panel_size = parent.get_panel_size();
    const auto panel_area_size = parent.get_panel_area_size();
    const auto mouse_state = panel.copyMouseState();

    program->setUniform("u_bounds",             shader_bounds.getX(), shader_bounds.getY(),
                                                shader_bounds.getWidth(), shader_bounds.getHeight());
    program->setUniform("u_resolution",         panel_size.x, panel_size.y,
                                                panel_area_size.x, panel_area_size.y);
    program->setUniform("u_rendering_scale",    static_cast<GLfloat>(OpenGLContext::getCurrentContext()->getRenderingScale()));
    program->setUniform("u_event_position",     mouse_state.eventPosition.x,
                                                mouse_state.eventPosition.y);
    program->setUniform("u_mouse_position",     mouse_state.mouseUpPosition.x,
                                                mouse_state.mouseUpPosition.y,
                                                mouse_state.mouseDownPosition.x,
                                                mouse_state.mouseDownPosition.y);
    program->setUniform("u_mouse_time",         mouse_state.eventTime,
                                                mouse_state.mouseUpTime,
                                                mouse_state.mouseDownTime, 0);
    program->setUniform("u_periodic_time",      parent.get_sin_time(),
                                                parent.get_saw_time());
    program->setUniform("u_flags",              static_cast<int>(mouse_state.lastEventType.index()),
                                                mouse_state.isDown,
                                                mouse_state.isToggled,
                                                mouse_state.isRightClick);
    program->setUniform("u_mouse_options",      mouse_state.wheelDelta.x, mouse_state.wheelDelta.y,
                                                mouse_state.scaleFactor);
}
void LiveShaderProgram::delete_program() { program = nullptr; }
