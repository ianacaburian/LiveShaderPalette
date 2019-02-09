/*
  ==============================================================================

    OpenGLRectangle.h
    Created: 9 Feb 2019 11:10:20am
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class OpenGLRectangle
{
    using GL = juce::OpenGLExtensionFunctions;
public:
    OpenGLRectangle();
    ~OpenGLRectangle() = default;
    void create();
    void render();
    void delete_vertex_objects();
private:
    static constexpr int elements_count = 6;
    GLuint vertex_arr_ID{}, vertex_buff_ID{}, index_buff_ID{};
};
