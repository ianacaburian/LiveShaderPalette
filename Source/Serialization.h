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
    Serialization();
    ~Serialization() = default;
    
    File get_vertex_file() const;
    File get_fragment_file() const;
private:
    File vertex_file{}, fragment_file{};
};
