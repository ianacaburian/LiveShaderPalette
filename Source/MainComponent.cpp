/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
#include "LiveShaderPanel.h"
#include "Console.h"

//==============================================================================

MainComponent::MainComponent()
{
    add_panels(0, tool_bar.get_num_panels());

    Desktop::getInstance().setDefaultLookAndFeel(&look);
    addAndMakeVisible(tool_bar);
    addAndMakeVisible(scroll_bar);
    scroll_bar.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    scroll_bar.setTextBoxIsEditable(true);
    scroll_bar.setRange(500., 10000.);

    period.setValue(1000);
    setSize(400, 300);
}
MainComponent::~MainComponent()
{
    open_console(false);
}
void MainComponent::resized()
{
    auto bounds = getLocalBounds().toFloat();
    tool_bar.setBounds(bounds.removeFromTop(proportionOfHeight(0.05f)).toNearestIntEdges());
    scroll_bar.setBounds(bounds.removeFromRight(proportionOfWidth(0.5f)).toNearestIntEdges());
    resize_panels(bounds);
}
void MainComponent::newOpenGLContextCreatedTopLevel()
{
}
void MainComponent::renderOpenGLTopLevel()
{
    const auto time = Time::currentTimeMillis();
    const auto period_double = static_cast<double>(period.getValue());
    sin_time = static_cast<float>(std::sin(time / period_double));
    saw_time = static_cast<float>(std::fmod(time, period_double) / period_double);
    tool_bar.log();
}
bool MainComponent::isInterestedInFileDrag(const StringArray& files) { return true; }
void MainComponent::filesDropped(const StringArray& files, int x, int y)
{
    visitChildren([&](auto& child) {
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
    // If dragged in a non-child area, load the same file into all panels.
    openGLContext.detach();
    visitChildren([&](auto& child) {
        if (auto* shader_panel = dynamic_cast<LiveShaderPanel*>(&child)) {
            shader_panel->load_shader_file(files[0]);
        }
    });
    openGLContext.attachTo(*this);
}
void MainComponent::timerCallback() { recompile_shaders(); }

//==============================================================================

void MainComponent::update_layout()
{
    MessageManager::callAsync([&] {
        openGLContext.detach();
        const auto initial_num_panels = static_cast<int>(getNumOpenGLChildComponents());
        const auto delta_panels = tool_bar.get_num_panels()  - initial_num_panels;
        if (0 < delta_panels) {
            add_panels(initial_num_panels, delta_panels);
        }
        else {
            for (int i = delta_panels; ++i <= 0;) {
                removeOpenGLComponent(initial_num_panels + delta_panels);
            }
        }
        resized();
        openGLContext.attachTo(*this);
    });
}
void MainComponent::open_console(const bool open)
{
    console = open ? std::make_unique<Console>(*this) : nullptr;
    visitChildren([open](auto& child) { child.enableLogging(open); });
}
Point<int> MainComponent::get_panel_area_size() const { return panel_area_size; }
Point<int> MainComponent::get_panel_size() const { return panel_size; }
std::pair<int, int> MainComponent::get_layout() const { return { tool_bar.get_layout().index(), tool_bar.get_num_panels() }; }
Value& MainComponent::get_compile_rate_val() { return tool_bar.get_compile_rate_val(); }
Value& MainComponent::get_period_val() { return period; }
float MainComponent::get_sin_time() const { return sin_time; }
float MainComponent::get_saw_time() const { return saw_time; }
bool MainComponent::is_live_compiling() const { return tool_bar.is_live_compiling(); }
bool MainComponent::is_console_open() const { return console != nullptr; }

//==============================================================================

MainComponent::Look::Look()
{
    setColour(ResizableWindow:: backgroundColourId, Colours::black);
    setColour(TextButton::      buttonColourId,     Colours::black);
    setColour(TextButton::      buttonOnColourId,   Colours::black);
    setColour(TextButton::      textColourOffId,    Colours::grey);
    setColour(TextEditor::      backgroundColourId, Colours::black);
    setColour(TextEditor::      outlineColourId,    Colours::grey);
    
    
    setColour(ScrollBar::thumbColourId, Colours::white);
    setColour(ScrollBar::trackColourId, Colours::red);
}

// MainComponent::private: =====================================================

void MainComponent::add_panels(const int initial_num_panels, const int num_panels_to_add)
{
    for (int i = initial_num_panels; i != initial_num_panels + num_panels_to_add; ++i) {
        addOpenGLComponent(new LiveShaderPanel{ *this, i });
    }
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
void MainComponent::resize_panels(Rectangle<float>& bounds)
{
    const auto int_bounds = bounds.toNearestIntEdges();
    panel_area_size = { int_bounds.getWidth(), int_bounds.getHeight() };
    DBG("int_bounds: " << int_bounds.toString());
    std::visit(Overloader{ 
        [&](const ToolBar::LayoutType::Tiled& l) {
            const auto panels_per_side = static_cast<int>(std::sqrt(l.num_panels));
            const auto panel_width = bounds.proportionOfWidth(1.f / panels_per_side);
            const auto panel_height = bounds.proportionOfHeight(1.f / panels_per_side);
            visitChildren([&, i = 0](auto& child) mutable {
                child.setBounds(Rectangle<float>{
                    i % panels_per_side * panel_width,
                    getHeight() - (i / panels_per_side * panel_height) - panel_height,
                    panel_width,
                    panel_height
                }.toNearestIntEdges());
                ++i;
            });
        },
        [&](const ToolBar::LayoutType::Rows& l) {
            const auto panel_height = bounds.proportionOfHeight(1.f / l.num_panels);
            visitChildren([&](auto& child) { child.setBounds(bounds.removeFromBottom(panel_height).toNearestIntEdges()); });
        },
        [&](const ToolBar::LayoutType::Columns& l) {
            const auto panel_width = bounds.proportionOfWidth(1.f / l.num_panels);
            visitChildren([&](auto& child) { child.setBounds(bounds.removeFromLeft(panel_width).toNearestIntEdges()); });
        }
    }, tool_bar.get_layout());
    
    const auto panel = getOpenGLComponent(0);
    panel_size = { panel->getWidth(), panel->getHeight() };
}
