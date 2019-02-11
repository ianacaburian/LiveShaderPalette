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

LiveShaderPanel::LiveShaderPanel(MainComponent& parent, const int component_ID)
: live_shader_program{ std::make_unique<LiveShaderProgram>(*this, serialization.get_vertex_file(), serialization.get_fragment_file()) }
, parent{ parent }
{
    setComponentID(String{ component_ID });
}
LiveShaderPanel::~LiveShaderPanel() {}

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
void LiveShaderPanel::recompile_shader()
{
    live_shader_program.reset(new LiveShaderProgram{
        *this, serialization.get_vertex_file(), serialization.get_fragment_file() });
}
float LiveShaderPanel::get_rendering_scale() const { return parent.get_rendering_scale(); }
float LiveShaderPanel::get_sin_time() const { return parent.get_sin_time(); }
