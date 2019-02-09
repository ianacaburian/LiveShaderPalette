/*
  ==============================================================================

    Serialization.h
    Created: 9 Feb 2019 10:40:57am
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class Serialization
{
public:
    Serialization(const char* vertex_filename, const char* fragment_filename);
    ~Serialization() = default;
    
    void load_shader_file(const String& file_path);
    File get_vertex_file() const;
    File get_fragment_file() const;
private:
    File vertex_file{}, fragment_file{};
};
