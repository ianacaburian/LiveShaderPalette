/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "OpenGLComponent.h"
#include "ToolBar.h"

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
    ~MainComponent();
    void resized() override;
    void renderOpenGLParent() override;
    std::optional<Rectangle<int>> getParentClippedDrawArea() override;
    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;
    void timerCallback() override;

    //==============================================================================

    void update_layout();
    float get_rendering_scale() const;
    float get_sin_time() const;
    
private:
    //==============================================================================
    
    struct Look : public LookAndFeel_V4 { explicit Look(); };
    
    //==============================================================================
    
    ToolBar tool_bar{ *this };
    Look look;
    Point<int> screen_resolution { 400, 300 };
    float sin_time{};
    
    //==============================================================================

    void add_panels(const int initial_num_panels, const int num_panels_to_add);
    void recompile_shaders();
    void resize_panels(Rectangle<float>& bounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
