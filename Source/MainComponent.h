/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "OpenGLComponent.h"
#include "ToolBar.h"
class Console;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public OpenGLParentComponent, public FileDragAndDropTarget, public Timer
{
public:
    //==============================================================================
    explicit MainComponent();
    ~MainComponent() = default;
    void resized() override;
    void newOpenGLContextCreatedParent() override;
    void renderOpenGLParent() override;
    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;
    void timerCallback() override;

    //==============================================================================

    void update_layout();
    void open_console(const bool open);
    std::pair<int, int> get_layout() const;
    float get_sin_time() const;
    float get_saw_time() const;
    
    
private:
    //==============================================================================
    
    struct Look : public LookAndFeel_V4 { explicit Look(); };
    
    //==============================================================================
    
    ToolBar tool_bar{ *this };
    std::unique_ptr<Console> console;
    Look look;
    Point<int> screen_resolution { 400, 300 };
    float sin_time{}, saw_time;
    int period = 1000;
    
    //==============================================================================

    void add_panels(const int initial_num_panels, const int num_panels_to_add);
    void recompile_shaders();
    void resize_panels(Rectangle<float>& bounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
