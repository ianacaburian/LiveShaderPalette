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
    visitChildren([&](auto& child){
        auto c_bounds = bounds.removeFromTop(proportionOfHeight(0.25f));
        DBG("rect: " << c_bounds.toString());
        child.setBounds(c_bounds);
    });
}
void MainComponent::newOpenGLContextCreatedParent() { desktop_scale = openGLContext.getRenderingScale(); }
void MainComponent::renderOpenGLParent()
{
    sin_time = static_cast<float>(std::sin(Time::currentTimeMillis() / 1000.));
    print_log();
}
float MainComponent::get_desktop_scale() const { return desktop_scale; }
float MainComponent::get_sin_time() const { return sin_time; }

// private: ====================================================================

void MainComponent::timerCallback() { recompile_shaders(); }
void MainComponent::init_buttons()
{
    live_compile.onClick = [this] {
        if (live_compile.getToggleState()) {
            startTimer(compile_interval_ms);
        }
        else {
            stopTimer();
        }
    };
    for (auto* b : buttons) {
        b->setClickingTogglesState(true);
        addAndMakeVisible(b);
    }
    auto&& laf = getLookAndFeel();
    laf.setColour(TextButton::buttonColourId, Colours::black);
    laf.setColour(TextButton::buttonOnColourId, Colours::black);
    laf.setColour(TextButton::textColourOnId, Colours::limegreen);
    laf.setColour(TextButton::textColourOffId, Colours::white);
    
    live_compile.triggerClick();
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
void MainComponent::print_log()
{
    if (print_ms_frame.getToggleState()
        || print_sin_time.getToggleState()) {
        frame_count++;
        if (const auto current_time = Time::currentTimeMillis();
            current_time - prev_time >= 1000.){
            ms_frame = 1000. / frame_count;
            if (print_ms_frame.getToggleState()) {
                DBG(String::formatted("%f ms/frame", ms_frame));
            }
            if (print_sin_time.getToggleState()) {
                DBG(String::formatted("sin(time) = %f", sin_time));
            }
            frame_count = 0;
            prev_time = current_time;
        }
    }
}
