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
: live_shader_program{ std::make_unique<LiveShaderProgram>(parent, *this, serialization.get_fragment_file()) }
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
    live_shader_program.reset(new LiveShaderProgram{ parent, *this, serialization.get_fragment_file() });
}

// LiveShaderPanel::FragmentShaderFile =========================================

LiveShaderPanel::FragmentShaderFile::FragmentShaderFile(const char* fragment_filename)
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
    fragment_file = shader_folder.getChildFile(fragment_filename);
}
void LiveShaderPanel::FragmentShaderFile::load_shader_file(const String& file_path)
{
    if (const auto file = File{ file_path};
        file.getFileExtension() == ".frag") {
        fragment_file = file;
    }
}
File LiveShaderPanel::FragmentShaderFile::get_fragment_file() const { return fragment_file; }
