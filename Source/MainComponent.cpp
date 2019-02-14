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

    period.setValue(1000);
    setSize(400, 300);
}
MainComponent::~MainComponent()
{
    open_console(false);
    openGLContext.detach();
    openGLContext.setRenderer(nullptr);
}
void MainComponent::resized()
{
    auto bounds = getLocalBounds().toFloat();
    tool_bar.setBounds(bounds.removeFromTop(proportionOfHeight(0.05f)).toNearestIntEdges());
    resize_panels(bounds);
}
void MainComponent::newOpenGLContextCreatedParent()
{
}
void MainComponent::renderOpenGLParent()
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
    visit_panels([&](auto& panel) {
        if (const auto panel_bounds = panel.getBounds();
            panel_bounds.contains(x, y)) {
            
            openGLContext.detach();
            panel.load_shader_file(files[0]);
            openGLContext.attachTo(*this);
            return;
        }
    });
    // If dragged in a non-panel area, load the same file into all panels.
    openGLContext.detach();
    visit_panels([&](auto& panel) { panel.load_shader_file(files[0]); });
    openGLContext.attachTo(*this);
}
void MainComponent::timerCallback() { recompile_shaders(); }

//==============================================================================

void MainComponent::initialize_fragment_file()
{
    auto shader_folder = File::getCurrentWorkingDirectory();
    while (!shader_folder.isRoot()) {
        if (shader_folder.getFileName() == JUCEApplication::getInstance()->getApplicationName()) {
            shader_folder = shader_folder.getChildFile("Source/Shaders/");
            break;
        }
        shader_folder = shader_folder.getParentDirectory();
    }
    if (! shader_folder.exists()) {
        jassertfalse;
    }
//    fragment_file = shader_folder.getChildFile(fragment_filename);
}
void MainComponent::update_layout()
{
    MessageManager::callAsync([&] {
        openGLContext.detach();
        const auto initial_num_panels = static_cast<int>(panels.size());
        const auto delta_panels = tool_bar.get_num_panels()  - initial_num_panels;
        if (0 < delta_panels) {
            add_panels(initial_num_panels, delta_panels);
        }
        else {
            const auto new_num_panels = initial_num_panels + delta_panels;
            for (int i = initial_num_panels; --i >= new_num_panels;) {
                removeOpenGLRendererComponent(*panels.erase(panels.begin() + i));
            }
        }
        resized();
        openGLContext.attachTo(*this);
    });
}
void MainComponent::open_console(const bool open)
{
    console = open ? std::make_unique<Console>(*this) : nullptr;
    visit_panels([open](auto& panel) { panel.enableLogging(open); });
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
}

// MainComponent::private: =====================================================

void MainComponent::add_panels(const int initial_num_panels, const int num_panels_to_add)
{
    for (int i = initial_num_panels; i != initial_num_panels + num_panels_to_add; ++i) {
        addOpenGLRendererComponent(panels.emplace_back(new LiveShaderPanel{ *this, i }));
    }
}
void MainComponent::visit_panels(std::function<void(LiveShaderPanel&)> f) { for (auto& p : panels) f(*p); }
void MainComponent::recompile_shaders()
{
    openGLContext.detach();
    visit_panels([&](auto& panel) { panel.recompile_shader(); });
    openGLContext.attachTo(*this);
}
void MainComponent::resize_panels(Rectangle<float>& bounds)
{
    const auto int_bounds = bounds.toNearestIntEdges();
    panel_area_size = { int_bounds.getWidth(), int_bounds.getHeight() };
    std::visit(Overloader{
        [&](const ToolBar::LayoutType::Tiled& l) {
            const auto panels_per_side = static_cast<int>(std::sqrt(l.num_panels));
            const auto panel_width = bounds.proportionOfWidth(1.f / panels_per_side);
            const auto panel_height = bounds.proportionOfHeight(1.f / panels_per_side);
            visit_panels([&, i = 0](auto& panel) mutable {
                panel.setBounds(Rectangle<float>{
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
            visit_panels([&](auto& panel) { panel.setBounds(bounds.removeFromBottom(panel_height).toNearestIntEdges()); });
        },
        [&](const ToolBar::LayoutType::Columns& l) {
            const auto panel_width = bounds.proportionOfWidth(1.f / l.num_panels);
            visit_panels([&](auto& panel) { panel.setBounds(bounds.removeFromLeft(panel_width).toNearestIntEdges()); });
        }
    }, tool_bar.get_layout());
    
    panel_size = { panels[0]->getWidth(), panels[0]->getHeight() };
}
