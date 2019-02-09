/*
  ==============================================================================

    OpenGLRectangle.cpp
    Created: 9 Feb 2019 11:10:20am
    Author:  Ian Caburian

  ==============================================================================
*/

#include "OpenGLRectangle.h"
OpenGLRectangle::OpenGLRectangle()
{
    
}
void OpenGLRectangle::create()
{
    constexpr auto positions_count = 8;
    const GLfloat positions[positions_count] {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };
    const GLuint elements[elements_count] {
        0, 1, 2,
        0, 2, 3
    };
    GL::glGenVertexArrays(1, &vertex_arr_ID);
    GL::glBindVertexArray(vertex_arr_ID);
    
    GL::glGenBuffers(1, &vertex_buff_ID);
    GL::glBindBuffer(GL_ARRAY_BUFFER, vertex_buff_ID);
    GL::glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positions_count, positions, GL_STATIC_DRAW);
    
    const auto position_attrib_id = 0, dimensions = 2;
    GL::glEnableVertexAttribArray(position_attrib_id);
    GL::glVertexAttribPointer(position_attrib_id, dimensions, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * dimensions, (const void*)0);     // For more that one attrib, can use a struct with a C++ macro to determine this void* offset.
    
    GL::glGenBuffers(1, &index_buff_ID);
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
    GL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements_count, elements, GL_STATIC_DRAW);
}
void OpenGLRectangle::render()
{
    // Binding the array simultaneously tells the GPU which vertex buffer to use as well as the specification of the buffer's memory layout.
    GL::glBindVertexArray(vertex_arr_ID);
    
    // The element (or index) buffer is still required to be bound at each draw call to specify how to use the positions in the buffer to draw the correct shape.
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
    glDrawElements(GL_TRIANGLES, elements_count, GL_UNSIGNED_INT, nullptr);
}
void OpenGLRectangle::delete_vertex_objects()
{
    GL::glDeleteVertexArrays(1, &vertex_arr_ID);
    GL::glDeleteBuffers(1, &vertex_buff_ID);
    GL::glDeleteBuffers(1, &index_buff_ID);
}
