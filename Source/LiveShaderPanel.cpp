/*
  ==============================================================================

    LiveShaderPanel.cpp
    Created: 8 Feb 2019 4:51:36am
    Author:  Ian Caburian

  ==============================================================================
*/

#include "LiveShaderPanel.h"
#include "LiveShaderProgram.h"
#include "MainComponent.h"

//==============================================================================

void LiveShaderPanel::MouseState::capture_event(const MouseEvent& mouse_event)
{
    mouse_position = mouse_event.position;
    mouse_down_position = mouse_event.mouseDownPosition;
    event_time      = std::fmodf(static_cast<double>(mouse_event.eventTime      .toMilliseconds()), static_cast<double>(std::numeric_limits<float>::max()));
    mouse_down_time = std::fmodf(static_cast<double>(mouse_event.mouseDownTime  .toMilliseconds()), static_cast<double>(std::numeric_limits<float>::max()));
}

//==============================================================================

LiveShaderPanel::LiveShaderPanel(MainComponent& parent, const int panel_ID)
: live_shader_program{ std::make_unique<LiveShaderProgram>(*this, serialization.get_vertex_file(), serialization.get_fragment_file()) }
, parent{ parent }
, panel_ID{ panel_ID } {}
LiveShaderPanel::~LiveShaderPanel() {}
void LiveShaderPanel::mouseDown(const MouseEvent& mouse_event)
{
    mouse_state.capture_event(mouse_event);
    mouse_state.is_toggled = ! mouse_state.is_toggled;
    mouse_state.is_down = true;
    mouse_state.is_right_click = mouse_event.mods.isRightButtonDown();
    DBG("mouseDown() | #" << panel_ID << " mouse_pos: " << mouse_event.position.toString());
}
void LiveShaderPanel::mouseUp(const MouseEvent& mouse_event)
{
    mouse_state.capture_event(mouse_event);
    mouse_state.is_down = false;
    DBG("mouseUp() | #" << panel_ID << " mouse_pos: " << mouse_event.position.toString());
}
//    wheel
// scale factor
// bools

void LiveShaderPanel::newOpenGLContextCreated()
{
    live_shader_program->create();
    rectangle.create();
}
void LiveShaderPanel::renderOpenGL()
{
    live_shader_program->render();
    rectangle.render();
}
void LiveShaderPanel::openGLContextClosing()
{
    live_shader_program->delete_program();
    rectangle.delete_vertex_objects();
}

//==============================================================================

void LiveShaderPanel::load_shader_file(const String& file_path)
{
    serialization.load_shader_file(file_path);
    recompile_shader();
}
void LiveShaderPanel::recompile_shader() { live_shader_program.reset(new LiveShaderProgram{ *this, serialization.get_vertex_file(), serialization.get_fragment_file() }); }
LiveShaderPanel::MouseState LiveShaderPanel::get_mouse_state() const { return mouse_state; }
float LiveShaderPanel::get_rendering_scale() const { return parent.get_rendering_scale(); }
float LiveShaderPanel::get_sin_time() const { return parent.get_sin_time(); }
int LiveShaderPanel::get_panel_ID() const { return panel_ID; }
