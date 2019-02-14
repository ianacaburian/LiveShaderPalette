/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "OpenGLComponent.h"
#include "ToolBar.h"
class LiveShaderPanel;
class Console;

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public OpenGLTopLevelComponent, public FileDragAndDropTarget, public Timer
{
public:
    //==============================================================================
    explicit MainComponent();
    ~MainComponent();
    void resized() override;
    void newOpenGLContextCreatedParent() override;
    void renderOpenGLParent() override;
    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;
    void timerCallback() override;

    //==============================================================================

    void update_layout();
    void open_console(const bool open);
    Point<int> get_panel_area_size() const;
    Point<int> get_panel_size() const;
    std::pair<int, int> get_layout() const;
    Value& get_compile_rate_val();
    Value& get_period_val();
    float get_sin_time() const;
    float get_saw_time() const;
    bool is_live_compiling() const;
    bool is_console_open() const;
    
    
private:
    //==============================================================================
    
    struct Look : public LookAndFeel_V4 { explicit Look(); };
    
    //==============================================================================
    
    std::vector<LiveShaderPanel*> panels;
    ToolBar tool_bar{ *this };
    std::unique_ptr<Console> console;
    Look look;
    Point<int> panel_area_size{}, panel_size{};
    float sin_time{}, saw_time;
    Value period;
    
    //==============================================================================
    
    void add_panels(const int initial_num_panels, const int num_panels_to_add);
    void visit_panels(std::function<void(LiveShaderPanel&)> f);
    void recompile_shaders();
    void resize_panels(Rectangle<float>& bounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
