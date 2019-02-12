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
//==============================================================================

ToolBar::ToolBar(MainComponent& parent) : parent{ parent }
{
    addAndMakeVisible(info_display);
    addAndMakeVisible(num_panels_txt);
    addAndMakeVisible(live_compile_btn);
    addAndMakeVisible(console_btn);
    live_compile_btn.setClickingTogglesState(true);
    console_btn.setClickingTogglesState(true);

    initialize_layout_buttons();
    set_component_callbacks();
    //    live_compile.triggerClick();
}
void ToolBar::resized()
{
    auto bounds = getLocalBounds();
    num_panels_txt.applyFontToAllText(mono_font(static_cast<float>(bounds.getHeight())));
    const auto button_width = proportionOfWidth(0.1f);
    for (auto* c : std::initializer_list<Component*>{
        &live_compile_btn, &num_panels_txt, &tiled_btn, &rows_btn, &columns_btn, &console_btn
    }) {
        c->setBounds(bounds.removeFromLeft(button_width));
    }
    info_display.setBounds(bounds);
}

//==============================================================================

void ToolBar::log() { info_display.log(); }
ToolBar::Layout ToolBar::get_layout() const { return layout; }
int ToolBar::get_num_panels() const { return std::visit([this](const auto& l){ return l.num_panels; }, layout); }

// ToolBar::InfoDisplay ========================================================

ToolBar::InfoDisplay::InfoDisplay(MainComponent& parent) : parent{ parent }
{
    setPaintingIsUnclipped(true);
    setOpaque(true);
}
void ToolBar::InfoDisplay::log()
{   // OpenGL thread - do not block or call message thread
    ++frame_count;
    if (const auto current_time = Time::currentTimeMillis();
        current_time - prev_time >= 1000.) {
        ms_frame = 1000. / frame_count;
        frame_count = 0;
        prev_time = current_time;
        MessageManager::callAsync([this]
        {
            repaint();
            if (parent.isLogging()) {
                auto s = String{ "p" };
                s << " Screen size: ( " << parent.getWidth() << ", " << parent.getHeight() << " )"
                  << "" // TODO: add panel size here
                  << " Rendering scale: " << parent.getRenderingScale() << " |"
                  << "\nSin time: " << parent.get_sin_time() << " |"
                  << " Saw time: " << parent.get_saw_time();
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

// ToolBar::LayoutType =========================================================

ToolBar::LayoutType::Tiled::Tiled(const int num_panels)
: num_panels{ std::max(1, static_cast<int>(std::pow(std::round(std::sqrt(num_panels)), 2))) } {}
ToolBar::LayoutType::Rows::Rows(const int num_panels)
: num_panels{ std::max(2, num_panels) } {}
ToolBar::LayoutType::Columns::Columns(const int num_panels)
: num_panels{ std::max(2, num_panels) } {}

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
    std::visit(Overloader{
        [this](const LayoutType::Tiled& l) {
            tiled_btn    .setToggleState(true, sendNotificationSync);
            num_panels_txt.setText(String{ l.num_panels });
        },
        [this](const LayoutType::Rows& l) {
            rows_btn      .setToggleState(true, sendNotificationSync);
            num_panels_txt.setText(String{ l.num_panels });
        },
        [this](const LayoutType::Columns& l) {
            columns_btn   .setToggleState(true, sendNotificationSync);
            num_panels_txt.setText(String{ l.num_panels });
        }
    }, layout);
}
void ToolBar::set_component_callbacks()
{
    live_compile_btn.onClick = [this] {
        if (live_compile_btn.getToggleState()) {
            parent.startTimer(compile_interval_ms);
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
}
Font ToolBar::mono_font(const float parent_height)
{
    return Font{ Font::getDefaultMonospacedFontName(), parent_height * 0.75f, 0 };
}
