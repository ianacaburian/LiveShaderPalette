/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <variant>
#include "OpenGLComponent.h"
class LiveShaderPanel;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public OpenGLParentComponent, public FileDragAndDropTarget, private Timer
{
public:
    //==============================================================================
    explicit MainComponent();
    ~MainComponent() = default;
    //==============================================================================
    void resized() override;
    void paint(Graphics& g) override;
    void renderOpenGLParent() override;
    std::optional<Rectangle<int>> getParentClippedDrawArea() override;
    bool isInterestedInFileDrag (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;
    float get_rendering_scale() const;
    float get_sin_time() const;
    
private:
    struct PanelArrangement
    {
        struct Square
        {
            Square(const int num_panels);
            int num_panels;
        };
        struct Rows{ int num_panels; };
        struct Columns{ int num_panels; };
        std::variant<Square, Rows, Columns> layout;
    };
    struct InfoDisplay
    {
        MainComponent& parent;
        double prev_time{}, ms_frame{};
        int frame_count{};

        InfoDisplay(MainComponent& parent);
        void log(std::function<void()> repaint);
        void paint(Graphics& g);
    };
    //==============================================================================
    std::vector<std::shared_ptr<LiveShaderPanel>> panels;
    PanelArrangement panel_arranger{ PanelArrangement::Rows{ 2 } };
    InfoDisplay info_display{ *this };
    Rectangle<int> toolbar_bounds;
    
    TextEditor num_panels_txt;
    
    TextButton
    live_compile_btn{ "Live Compile" },
    square_btn{ "Square" }, rows_btn{ "Rows" }, columns_btn{ "Columns" };
    
    Point<int> screen_resolution { 400, 300 };
    float sin_time{};
    int compile_interval_ms = 2000;
    
    //==============================================================================

    void timerCallback() override;
    void init_buttons(LookAndFeel& look);
    void recompile_shaders();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
