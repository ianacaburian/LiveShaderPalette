/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "LiveShaderPanel.h"
//==============================================================================
MainComponent::MainComponent()
{
    for (int i = 0; i != 4; ++i) {
        addChildComponent(panels.emplace_back(std::make_shared<LiveShaderPanel>(*this, i)));
    }
    auto& look = getLookAndFeel();
    look.setColour(ResizableWindow::backgroundColourId, Colours::black);
    init_buttons(look);
    
    for (auto* c : std::initializer_list<Button*>{
        &square_btn, &rows_btn, &columns_btn
    }) {
        addAndMakeVisible(c);
        c->setClickingTogglesState(true);
    }
    addAndMakeVisible(num_panels_txt);
    square_btn.setRadioGroupId(1);
    rows_btn.setRadioGroupId(1);
    columns_btn.setRadioGroupId(1);
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, PanelArrangement::Square>)
            square_btn.setToggleState(true, sendNotificationSync);
        else if constexpr (std::is_same_v<T, PanelArrangement::Rows>)
            rows_btn.setToggleState(true, sendNotificationSync);
        else if constexpr (std::is_same_v<T, PanelArrangement::Columns>)
            columns_btn.setToggleState(true, sendNotificationSync);
    }, panel_arranger.layout);
    
    num_panels_txt.onReturnKey = [this] {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, PanelArrangement::Square>)
                panel_arranger.layout = PanelArrangement::Square{ num_panels_txt.getText().getIntValue() };
            else
                arg.num_panels = num_panels_txt.getText().getIntValue();
        }, panel_arranger.layout);
    };
    square_btn.onClick = [this] {
        std::visit([this](auto&& v){ panel_arranger.layout = PanelArrangement::Square{ v.num_panels }; }, panel_arranger.layout);
//        panel_arranger.layout = PanelArrangement::Square{ panel_arranger.layout.value().num_panels };
    };
    rows_btn.onClick = [this] {
        std::visit([this](auto&& v){ panel_arranger.layout = PanelArrangement::Rows{ v.num_panels }; }, panel_arranger.layout);

    };
    columns_btn.onClick = [this] {
        std::visit([this](auto&& v){ panel_arranger.layout = PanelArrangement::Columns{ v.num_panels }; }, panel_arranger.layout);
    };
    setSize(screen_resolution.x, screen_resolution.y);
}
MainComponent::PanelArrangement::Square::Square(const int num_panels) : num_panels{ (num_panels < 4 ? 1 : num_panels / 4 * 4) } {}
void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    toolbar_bounds = bounds.removeFromTop(proportionOfHeight(0.05f));
    
    auto button_bounds = toolbar_bounds;
    const auto button_width = proportionOfWidth(0.1f);
    live_compile_btn.setBounds(button_bounds.removeFromLeft(button_width));
    num_panels_txt.setBounds(button_bounds.removeFromLeft(button_width));
    square_btn.setBounds(button_bounds.removeFromLeft(button_width));
    rows_btn.setBounds(button_bounds.removeFromLeft(button_width));
    columns_btn.setBounds(button_bounds.removeFromLeft(button_width));

    
    const auto panel_area_height = bounds.proportionOfHeight(0.25f);
    
    visitChildren([&, i = 0](auto& child) mutable {
        child.setBounds(bounds.removeFromTop(panel_area_height));
        i = i % 2;
    });
}
void MainComponent::paint(Graphics& g) { info_display.paint(g); }
void MainComponent::renderOpenGLParent()
{
    sin_time = static_cast<float>(std::sin(Time::currentTimeMillis() / 1000.));
    info_display.log([this]{ repaint(toolbar_bounds.toNearestIntEdges()); });
}
std::optional<Rectangle<int>> MainComponent::getParentClippedDrawArea() { return toolbar_bounds; }
bool MainComponent::isInterestedInFileDrag(const StringArray& files) { return true; }
void MainComponent::filesDropped(const StringArray& files, int x, int y)
{
    visitChildren([&](auto& child){
        if (const auto child_bounds = child.getBounds();
            child_bounds.contains(x, y)) {
            
            openGLContext.detach();
            if (auto* shader_panel = dynamic_cast<LiveShaderPanel*>(&child)) {
                shader_panel->load_shader_file(files[0]);
            }
            openGLContext.attachTo(*this);
            return;
        }
    });
}
float MainComponent::get_rendering_scale() const { return getRenderingScale(); }
float MainComponent::get_sin_time() const { return sin_time; }

// private: ====================================================================
MainComponent::InfoDisplay::InfoDisplay(MainComponent& parent) : parent{ parent } {}
void MainComponent::InfoDisplay::log(std::function<void()> repaint)
{   // OpenGL thread - do not block
    ++frame_count;
    if (const auto current_time = Time::currentTimeMillis();
        current_time - prev_time >= 1000.){
        ms_frame = 1000. / frame_count;
        frame_count = 0;
        prev_time = current_time;
        MessageManager::callAsync(repaint);
    }
}
void MainComponent::InfoDisplay::paint(Graphics& g)
{
    g.setColour(Colours::black);
    g.fillRect(parent.toolbar_bounds);
    g.setColour(Colours::white);
    
    g.setFont(Font{
        Font::getDefaultMonospacedFontName(),
        static_cast<float>(parent.toolbar_bounds.proportionOfHeight(0.75f)),
        0
    });
//    g.drawText(String::formatted("%.2f ms/frame | sin(t):% 2.2f", ms_frame, parent.sin_time),
//               parent.toolbar_bounds, Justification::topRight);
    std::visit([&](auto& v){
        g.drawText(String::formatted("%.2f ms/frame | np:%d", ms_frame, v.num_panels),
                   parent.toolbar_bounds, Justification::topRight);

    }, parent.panel_arranger.layout);
}
void MainComponent::timerCallback() { recompile_shaders(); }
void MainComponent::init_buttons(LookAndFeel& look)
{
    live_compile_btn.setClickingTogglesState(true);
    live_compile_btn.onClick = [this] {
        if (live_compile_btn.getToggleState()) {
            startTimer(compile_interval_ms);
        }
        else {
            stopTimer();
        }
    };
    look.setColour(TextButton::buttonColourId, Colours::black);
    look.setColour(TextButton::buttonOnColourId, Colours::black);
    look.setColour(TextButton::textColourOffId, Colours::grey);
    addAndMakeVisible(live_compile_btn);
    //    live_compile.triggerClick();
}
void MainComponent::recompile_shaders()
{
    openGLContext.detach();
    visitChildren([&](auto& child) {
        if (auto* panel = dynamic_cast<LiveShaderPanel*>(&child)) {
            panel->recompile_shader();
        }
    });
    openGLContext.attachTo(*this);
}

