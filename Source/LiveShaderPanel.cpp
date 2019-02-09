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
: parent{ parent }
, live_shader_program{ std::make_unique<LiveShaderProgram>(*this, serialization.get_vertex_file(), serialization.get_fragment_file()) }
, component_ID{ component_ID }
{
}
LiveShaderPanel::~LiveShaderPanel()
{
}
void LiveShaderPanel::mouseDown(const MouseEvent& mouse_event)
{
    DBG("Component ID: " << component_ID << " mouse_pos: " << mouse_event.position.toString());
}
void LiveShaderPanel::newOpenGLContextCreated()
{
    live_shader_program->create();
    // Explicitly generate a vertex array (rather than use the default created one) to specify the vertex buffer layout in each draw call.
    // Otherwise, using the default requires that each draw call must bind the vertex buffer, enable the attrib array, and specify the attrib pointers.
    GL::glGenVertexArrays(1, &vertex_arr_ID);
    GL::glBindVertexArray(vertex_arr_ID);
    
    // Load the (x, y) positions (which are just the corners of the screen), into a static area of GPU memory.
    GL::glGenBuffers(1, &vertex_buff_ID);
    GL::glBindBuffer(GL_ARRAY_BUFFER, vertex_buff_ID);
    GL::glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positions_count, positions, GL_STATIC_DRAW);
    
    // Specify the attribute layout of the currently bound vertex buffer object (there's only one attribute - position).
    // The currently instantiated vertex array object will use this specification for each draw call.
    GL::glEnableVertexAttribArray(pos_attrib_id);
    GL::glVertexAttribPointer(pos_attrib_id, num_floats_per_pos_attrib, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * num_floats_per_pos_attrib, (const void*)0);     // For more that one attrib, can use a struct with a C++ macro to determine this void* offset.
    
    // Specify which (x, y) positions are used to draw two triangles in the shape of a rectangle (the screen) using the positions in the currently bound vertex buffer object.
    GL::glGenBuffers(1, &index_buff_ID);
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
    GL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements_count, elements, GL_STATIC_DRAW);

    // No need to unbind the array, buffer, or shader program thanks to JUCE.
}
void LiveShaderPanel::renderOpenGL()
{
    live_shader_program->render();
    
    // Binding the array simultaneously tells the GPU which vertex buffer to use as well as the specification of the buffer's memory layout.
    GL::glBindVertexArray(vertex_arr_ID);
    
    // The element (or index) buffer is still required to be bound at each draw call to specify how to use the positions in the buffer to draw the correct shape.
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
    glDrawElements(GL_TRIANGLES, elements_count, GL_UNSIGNED_INT, nullptr);
}
void LiveShaderPanel::openGLContextClosing()
{   // Destroying GL objects require that the openGL_context is not yet destroyed, i.e. cannot delete in the destructor.
    live_shader_program->delete_program();
    GL::glDeleteVertexArrays(1, &vertex_arr_ID);
    GL::glDeleteBuffers(1, &vertex_buff_ID);
    GL::glDeleteBuffers(1, &index_buff_ID);
}
void LiveShaderPanel::recompile_shader()
{
    live_shader_program.reset(new LiveShaderProgram{ *this, serialization.get_vertex_file(), serialization.get_fragment_file() });
}
float LiveShaderPanel::get_desktop_scale() const { return parent.get_desktop_scale(); }
float LiveShaderPanel::get_sin_time() const { return parent.get_sin_time(); }
int LiveShaderPanel::get_ID() const { return component_ID; }
