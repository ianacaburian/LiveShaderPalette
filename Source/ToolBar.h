/*
  ==============================================================================

    ToolBar.h
    Created: 10 Feb 2019 6:45:24pm
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <variant>
class MainComponent;

//==============================================================================

class ToolBar : public Component
{
public:
    struct LayoutType
    {
        struct Tiled    { Tiled     (const int num_panels);    int num_panels; };
        struct Rows     { Rows      (const int num_panels);    int num_panels; };
        struct Columns  { Columns   (const int num_panels);    int num_panels; };
    };
    using Layout = std::variant<LayoutType::Tiled, LayoutType::Rows, LayoutType::Columns>;

    //==============================================================================

    explicit ToolBar(MainComponent& parent);
    ~ToolBar() = default;
    void resized() override;

    //==============================================================================

    void log();
    Layout get_layout() const;
    static Font mono_font(const float parent_height);
    Value& get_compile_rate_val();
    int get_num_panels() const;
    bool is_live_compiling() const;
    

private:
    //==============================================================================

    class InfoDisplay : public Component
    {
    public:
        explicit InfoDisplay(MainComponent& parent);
        ~InfoDisplay() = default;
        void paint(Graphics& g) override;
        void log();
    private:
        double prev_time{}, ms_frame{};
        int frame_count{};
        MainComponent& parent;
    };
    
    //==============================================================================

    MainComponent& parent;
    InfoDisplay info_display{ parent };
    Layout layout{ LayoutType::Tiled{ 1 }};
    Rectangle<int> toolbar_bounds;
    TextEditor num_panels_txt;
    TextButton
    tiled_btn           { "Tiled" },
    rows_btn            { "Rows" },
    columns_btn         { "Columns" },
    live_compile_btn    { "Live Compile" },
    console_btn         { "Console" },
    folder_btn          { "Folder" };
    Value compile_rate_val;

    //==============================================================================

    void initialize_layout_buttons();
    void set_component_callbacks();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolBar)
};
