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
    init_buttons();
    setSize(screen_resolution.x, screen_resolution.y);
}
MainComponent::~MainComponent()
{
    DBG("MainComponentDestructor");
}
void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    toolbar_bounds = bounds.removeFromTop(proportionOfHeight(0.05f));
    
    auto button_bounds = toolbar_bounds;
    const auto button_width = proportionOfWidth(0.2f);
    live_compile_btn.setBounds(button_bounds.removeFromLeft(button_width));
    layout_btn.setBounds(button_bounds.removeFromLeft(button_width));
    
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
    DBG("filesDropped() | x: " << x << ", y: " << y);
    auto count = 0;
    for (auto& f : files) {
        DBG("#" << ++count << " " << f);
    }
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
    g.setFont(Font{ Font::getDefaultMonospacedFontName(),  14, 0 });
    g.drawText(String::formatted("%.2f ms/frame | sin(time) = % 2.2f", ms_frame, parent.sin_time),
               parent.toolbar_bounds, Justification::topRight);
}
void MainComponent::timerCallback() { recompile_shaders(); }
void MainComponent::init_buttons()
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
    layout_btn.onClick = [this] {
        
    };
    auto&& laf = getLookAndFeel();
    laf.setColour(TextButton::buttonColourId, Colours::black);
    laf.setColour(TextButton::buttonOnColourId, Colours::black);
    live_compile_btn.setColour(TextButton::textColourOffId, Colours::grey);
    addAndMakeVisible(live_compile_btn);
    addAndMakeVisible(layout_btn);
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

void MainComponent::choose_layout()
{
    DialogWindow::LaunchOptions dialog;
    dialog.content.setNonOwned(&selector);
    dialog.dialogTitle = "Audio Settings";
    dialog.componentToCentreAround = this;
    dialog.dialogBackgroundColour = getLookAndFeel().findColour(ResizableWindow::backgroundColourId);
    dialog.useNativeTitleBar = false;
    dialog.resizable = false;
    dialog.useBottomRightCornerResizer = false;
    dialog.runModal();
}
