/*
  ==============================================================================

    ToolBar.cpp
    Created: 10 Feb 2019 6:45:24pm
    Author:  Ian Caburian

  ==============================================================================
*/

#include "ToolBar.h"
#include "MainComponent.h"
#include "Console.h"

// ToolBar::LayoutType =========================================================

ToolBar::LayoutType::Tiled::Tiled(const int num_panels)
: num_panels{ std::max(1, static_cast<int>(std::pow(std::ceil(std::sqrt(num_panels)), 2))) } {}
ToolBar::LayoutType::Rows::Rows(const int num_panels)
: num_panels{ std::max(2, num_panels) } {}
ToolBar::LayoutType::Columns::Columns(const int num_panels)
: num_panels{ std::max(2, num_panels) } {}

// ToolBar =====================================================================

ToolBar::ToolBar(MainComponent& parent) : parent{ parent }
{
    for (auto* c : std::initializer_list<Component*>{
        &info_display, &num_panels_txt, &console_btn, &live_compile_btn, &refresh_folder_btn
    }) {
        addAndMakeVisible(c);
    }
    live_compile_btn.setClickingTogglesState(true);
    console_btn.setClickingTogglesState(true);
    refresh_folder_btn.setColour(TextButton::textColourOffId, Colours::white);
    
    initialize_layout_buttons();
    set_component_callbacks();
    compile_rate_val.setValue(1000);
}
void ToolBar::resized()
{
    auto bounds = getLocalBounds();
    num_panels_txt.applyFontToAllText(mono_font(static_cast<float>(bounds.getHeight())));
    const auto button_width = proportionOfWidth(0.11f);
    for (auto* c : std::initializer_list<Component*>{
        &num_panels_txt, &tiled_btn, &rows_btn, &columns_btn,
        &live_compile_btn, &console_btn, &refresh_folder_btn
    }) {
        c->setBounds(bounds.removeFromLeft(button_width));
    }
    info_display.setBounds(bounds);
}

//==============================================================================

void ToolBar::log() { info_display.log(); }
ToolBar::Layout ToolBar::get_layout() const { return layout; }
Value& ToolBar::get_compile_rate_val() { return compile_rate_val; }
Value& ToolBar::get_console_btn_val() { return console_btn.getToggleStateValue(); }
int ToolBar::get_num_panels() const { return std::visit([this](const auto& l){ return l.num_panels; }, layout); }
bool ToolBar::is_live_compiling() const { return live_compile_btn.getToggleState(); }

// ToolBar::InfoDisplay ========================================================

ToolBar::InfoDisplay::InfoDisplay(MainComponent& parent) : parent{ parent }
{
    setPaintingIsUnclipped(true);
    setOpaque(true);
}
void ToolBar::InfoDisplay::log()
{   // OpenGL thread - do not block or sync call message thread
    ++frame_count;
    if (const auto current_time = Time::currentTimeMillis();
        current_time - prev_time >= 1000.) {
        ms_frame = 1000. / frame_count;
        frame_count = 0;
        prev_time = current_time;
        const auto rendering_scale = OpenGLContext::getCurrentContext()->getRenderingScale();
        MessageManager::callAsync([this, rendering_scale]
        {
            repaint();
            if (parent.is_console_open()) {
                auto strf1 = [](const float f) { return String::formatted("% .2f", f); };
                auto strd = [](const int d) { return String{ d }.paddedLeft(' ', 4); };
                const auto panel_size = parent.get_panel_size();
                const auto panel_area_size = parent.get_panel_area_size();
                auto s = String{ "p " };
                s << "openGLContext.getRenderingScale(): " << strf1(rendering_scale)
                  << "\n   Panel:  " << strd(panel_size.x) << " " << strd(panel_size.y)
                  << "\n  Screen:  " << strd(panel_area_size.x) << " " << strd(panel_area_size.y)
                  << "\nSin time: " << strf1(parent.get_sin_time())
                  << "\nSaw time: " << strf1(parent.get_saw_time());
                
                Logger::writeToLog(s);
            }
        });
    }
}
void ToolBar::InfoDisplay::paint(Graphics& g)
{
    auto bounds = getLocalBounds();
    g.setColour(Colours::black);
    g.fillRect(bounds);
    g.setColour(Colours::white);
    g.setFont(mono_font(static_cast<float>(bounds.getHeight())));
    g.drawText(String::formatted("%.2f ms/frame", ms_frame),
               bounds, Justification::centredRight);
}

// ToolBar::private: ===========================================================

void ToolBar::initialize_layout_buttons()
{
    for (auto* c : std::initializer_list<Button*>{
        &tiled_btn, &rows_btn, &columns_btn
    }) {
        addAndMakeVisible(c);
        c->setClickingTogglesState(true);
        c->setRadioGroupId(1);
    }
    tiled_btn.setConnectedEdges(Button::ConnectedOnLeft | Button::ConnectedOnRight);
    rows_btn.setConnectedEdges(Button::ConnectedOnLeft | Button::ConnectedOnRight);
    columns_btn.setConnectedEdges(Button::ConnectedOnLeft);

    std::visit([this](const auto& l) {
        using T = std::decay_t<decltype(l)>;
         (std::is_same_v<T, LayoutType::Tiled> ? tiled_btn
        : std::is_same_v<T, LayoutType::Rows>  ? rows_btn
        : columns_btn).setToggleState(true, sendNotificationSync);
        num_panels_txt.setText(String{ l.num_panels });
    }, layout);

}
void ToolBar::set_component_callbacks()
{
    live_compile_btn.onClick = [this] {
        if (live_compile_btn.getToggleState()) {
            parent.startTimer(compile_rate_val.getValue());
        }
        else {
            parent.stopTimer();
        }
    };
    num_panels_txt.onReturnKey = [this] {
        std::visit([this](const auto& l) {
            layout = std::decay_t<decltype(l)>{ num_panels_txt.getText().getIntValue() };
            num_panels_txt.setText(String{ l.num_panels });
        }, layout);
        parent.update_layout();
    };
    tiled_btn.onClick = [this] {
        std::visit([this](const auto& l) {
            layout = LayoutType::Tiled{ l.num_panels };
            num_panels_txt.setText(String{ l.num_panels });
        }, layout);
        parent.update_layout();
    };
    rows_btn.onClick = [this] {
        std::visit([this](const auto& l) {
            layout = LayoutType::Rows{ l.num_panels };
            num_panels_txt.setText(String{ l.num_panels });
        }, layout);
        parent.update_layout();
    };
    columns_btn.onClick = [this] {
        std::visit([this](const auto& l) {
            layout = LayoutType::Columns{ l.num_panels };
            num_panels_txt.setText(String{ l.num_panels });
        }, layout);
        parent.update_layout();
    };
    console_btn.onClick = [this] {
        parent.open_console(console_btn.getToggleState());
    };
    refresh_folder_btn.onClick = [this] {
        parent.refresh_fragment_folder();
    };
}
Font ToolBar::mono_font(const float parent_height)
{
    return Font{ Font::getDefaultMonospacedFontName(), parent_height * 0.75f, 0 };
}
