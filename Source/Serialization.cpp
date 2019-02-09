/*
  ==============================================================================

    Serialization.cpp
    Created: 9 Feb 2019 10:40:57am
    Author:  Ian Caburian

  ==============================================================================
*/

#include "Serialization.h"
Serialization::Serialization(const char* vertex_filename, const char* fragment_filename)
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
    vertex_file = shader_folder.getChildFile(vertex_filename);
    fragment_file = shader_folder.getChildFile(fragment_filename);
}
void Serialization::load_shader_file(const String& file_path)
{
    if (const auto file = File{ file_path};
        file.getFileExtension() == ".frag") {
        fragment_file = file;
    }
}
File Serialization::get_vertex_file() const { return vertex_file; }
File Serialization::get_fragment_file() const { return fragment_file; }
