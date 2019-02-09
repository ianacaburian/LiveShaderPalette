/*
  ==============================================================================

    Serialization.cpp
    Created: 9 Feb 2019 10:40:57am
    Author:  Ian Caburian

  ==============================================================================
*/

#include "Serialization.h"
Serialization::Serialization()
{
    auto shader_folder = File::getCurrentWorkingDirectory();
    while (!shader_folder.isRoot()) {
        if (shader_folder.getFileName() == JUCEApplication::getInstance()->getApplicationName()) {
            shader_folder = shader_folder.getChildFile("Source/Shaders/");
            break;
        }
        shader_folder = shader_folder.getParentDirectory();
    }
    if (! shader_folder.exists()) {
        jassertfalse;
    }
    vertex_file = shader_folder.getChildFile("Vert.vert");
    fragment_file = shader_folder.getChildFile("Frag.frag");
}
File Serialization::get_vertex_file() const { return vertex_file; }
File Serialization::get_fragment_file() const { return fragment_file; }
