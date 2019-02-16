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
    collect_fragment_files();
    add_panels(0, tool_bar.get_num_panels());
    Desktop::getInstance().setDefaultLookAndFeel(&look);
    addAndMakeVisible(tool_bar);

    period.setValue(1000);
    setSize(640, 480 * (1 + tool_bar_scalar));
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
    tool_bar.setBounds(bounds.removeFromTop(proportionOfHeight(tool_bar_scalar)).toNearestIntEdges());
    resize_panels(bounds);
}
void MainComponent::newOpenGLContextCreatedParent()
{
    shader_compilation_errors = false;
}
void MainComponent::checkContextCreation()
{
    if (! shader_compilation_errors) {
        MessageManager::callAsync([]{ Logger::writeToLog("s All shaders compiled successfully."); });
    }
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
    if (tool_bar.getBounds().contains(x, y)) {
        openGLContext.detach();
        visit_panels([&](auto& panel) { panel.load_shader_file(files[0]); });
        openGLContext.attachTo(*this);
        return;
    }
    visit_panels([&](auto& panel) {
        if (const auto panel_bounds = panel.getBounds();
            panel_bounds.contains(x, y)) {
            
            openGLContext.detach();
            panel.load_shader_file(files[0]);
            openGLContext.attachTo(*this);
            return;
        }
    });
}
void MainComponent::timerCallback() { recompile_shaders(); }

//==============================================================================

void MainComponent::collect_fragment_files()
{
    const auto get_shader_folder = File::getSpecialLocation(File::SpecialLocationType::userDocumentsDirectory)
                                        .getChildFile(JUCEApplication::getInstance()->getApplicationName())
                                        .getChildFile("CWD");
    if (const auto result = get_shader_folder.createDirectory();
        result.wasOk()) {
        shader_folder = get_shader_folder;
        refresh_fragment_folder();
    }
    else {
        const auto error = String{ "Could not create a folder in your Documents folder! Error: " }
                         + result.getErrorMessage();
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Load error", error);
    }
}
void MainComponent::refresh_fragment_folder()
{
    auto refresh_fragment_files = [this]{
        fragment_files.clear();
        auto dir = DirectoryIterator{ shader_folder, true, "*.frag", File::findFiles };
        while (dir.next()) {
            fragment_files.push_back(dir.getFile());
        }
    };
    if (openGLContext.isAttached()) {
        refresh_fragment_files();
        if (const auto num_fragments = static_cast<int>(fragment_files.size());
            num_fragments) {
            visit_panels([&, i = 0](auto& panel) mutable {
                panel.load_shader_file(fragment_files[i % num_fragments]);
                ++i;
            });
            recompile_shaders();
        }
    }
    else {
        refresh_fragment_files();
    }
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
    console = open ? std::make_unique<Console>(*this, tool_bar) : nullptr;
}
void MainComponent::report_shader_compilation_error() { shader_compilation_errors = true; }
Point<int> MainComponent::get_panel_area_size() const { return panel_area_size; }
Point<int> MainComponent::get_panel_size() const { return panel_size; }
std::pair<int, int> MainComponent::get_layout() const { return { tool_bar.get_layout().index(), tool_bar.get_num_panels() }; }
Value& MainComponent::get_period_val() { return period; }
float MainComponent::get_sin_time() const { return sin_time; }
float MainComponent::get_saw_time() const { return saw_time; }
bool MainComponent::is_console_open() const { return console != nullptr; }

// MainComponent::Look =========================================================

MainComponent::Look::Look()
{
    setColour(ResizableWindow:: backgroundColourId, Colours::black);
    setColour(TextButton::      buttonColourId,     Colours::black);
    setColour(TextButton::      buttonOnColourId,   Colours::black);
    setColour(TextButton::      textColourOffId,    Colours::grey);
    setColour(TextEditor::      backgroundColourId, Colours::black);
    setColour(TextEditor::      outlineColourId,    Colours::grey);
    setColour(Label::textColourId, Colours::white);
}

// MainComponent::private: =====================================================

void MainComponent::add_panels(const int initial_num_panels, const int num_panels_to_add)
{
    const auto num_fragments = static_cast<int>(fragment_files.size());
    for (int i = initial_num_panels; i != initial_num_panels + num_panels_to_add; ++i) {
        auto* new_panel = panels.emplace_back(new LiveShaderPanel{ *this, i });
        if (num_fragments) {
            new_panel->load_shader_file(fragment_files[i % num_fragments]);
        }
        addOpenGLRendererComponent(new_panel);
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
