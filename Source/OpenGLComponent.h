/*
  ==============================================================================

    OpenGLComponent.h
    Created: 8 Feb 2019 8:34:52am
    Authors:  Ian Caburian & Marius Metzger

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <variant>
#include <optional>
class OpenGLChildComponent : public MouseListener, public OpenGLRenderer
{
public:
    explicit OpenGLChildComponent() = default;
    ~OpenGLChildComponent() = default;
    Rectangle<int> getBounds() const { return bounds; }
    int getX() const { return bounds.getX(); }
    int getY() const { return bounds.getY(); }
    int getBottom() const { return bounds.getBottom(); }
    int getWidth() const { return bounds.getWidth(); }
    int getHeight() const { return bounds.getHeight(); }
    void setBounds(const Rectangle<int>& new_bounds) { bounds = new_bounds; }
private:
    Rectangle<int> bounds;
};

class OpenGLParentComponent : public Component, public OpenGLRenderer
{
public:
    explicit OpenGLParentComponent()
    {
        setPaintingIsUnclipped(true);
        setOpaque(true);

        openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
        openGLContext.setContinuousRepainting(true);
        openGLContext.setComponentPaintingEnabled(true);
        openGLContext.setMultisamplingEnabled(true);                            // Eliminates flickering, no idea how or why.
        openGLContext.setRenderer(this);
        openGLContext.attachTo(*this);        
    }
    ~OpenGLParentComponent()
    {
        openGLContext.detach();
        openGLContext.setRenderer(nullptr);
    }
    void mouseMove(const MouseEvent& m) override final { mouseCall(m, MouseType::Move{}); }
    void mouseEnter(const MouseEvent& m) override final { mouseCall(m, MouseType::Enter{}); }
    void mouseExit(const MouseEvent& m) override final { mouseCall(m, MouseType::Exit{}); }
    void mouseDown(const MouseEvent& m) override final { mouseCall(m, MouseType::Down{}); }
    void mouseDrag(const MouseEvent& m) override final { mouseCall(m, MouseType::Drag{}); }
    void mouseUp(const MouseEvent& m) override final { mouseCall(m, MouseType::Up{}); }
    void mouseDoubleClick(const MouseEvent& m) override final { mouseCall(m, MouseType::DoubleClick{}); }
    void mouseWheelMove(const MouseEvent& m, const MouseWheelDetails& w) override final { mouseCall(m, MouseType::WheelMove{ w }); }
    void mouseMagnify(const MouseEvent& m, float s) override final { mouseCall(m, MouseType::Magnify{ s }); }

protected:
    OpenGLContext openGLContext;
    
    virtual void newOpenGLContextCreatedParent() {}
    virtual void renderOpenGLParent() {}
    virtual void openGLContextClosingParent() {}
    virtual std::optional<Rectangle<int>> getParentClippedDrawArea() = 0;
    
    void addChildComponent(const std::shared_ptr<OpenGLChildComponent> new_child) { children.push_back(new_child); }
    void visitChildren(std::function<void(OpenGLChildComponent&)> f) { for (auto& child : children) f(*child); }
    float getRenderingScale() const { return renderingScale; }
    
private:
    std::vector<std::shared_ptr<OpenGLChildComponent>> children;
    float renderingScale{};
    
    void newOpenGLContextCreated() override final
    {
        newOpenGLContextCreatedParent();
        visitChildren([](auto& child) { child.newOpenGLContextCreated(); });
    }
    void renderOpenGL() override final
    {
        renderingScale = openGLContext.getRenderingScale();

        glEnable(GL_SCISSOR_TEST); // This needs to be called here, and has no effect in the initialization function for some reason
        
        clipDrawArea(getParentClippedDrawArea());
        renderOpenGLParent();
        
        visitChildren([this](auto& child) {
            clipDrawArea(child.getBounds());
            child.renderOpenGL();
        });
    }
    void openGLContextClosing() override final
    {
        visitChildren([](auto& child) { child.openGLContextClosing(); });
        openGLContextClosingParent();
    }
    void clipDrawArea(const std::optional<Rectangle<int>>& area)
    {
        if (area) {
            const auto x = area.value().getX() * renderingScale;
            const auto y = (getHeight() - area.value().getBottom()) * renderingScale;
            const auto w = area.value().getWidth() * renderingScale;
            const auto h = area.value().getHeight() * renderingScale;
            glViewport(x, y, w, h);
            glScissor(x, y, w, h);
        }
    }
    // Mouse Forwarding ========================================================
    struct MouseType
    {
        struct Move{};
        struct Enter{};
        struct Exit{};
        struct Down{};
        struct Drag{};
        struct Up{};
        struct DoubleClick{};
        struct WheelMove{ const MouseWheelDetails& wheel; };
        struct Magnify{ float scaleFactor; };
    };
    void mouseCall(const MouseEvent& mouseEvent,
                   const std::variant<MouseType::Move,
                                      MouseType::Enter,
                                      MouseType::Exit,
                                      MouseType::Down,
                                      MouseType::Drag,
                                      MouseType::Up,
                                      MouseType::DoubleClick,
                                      MouseType::WheelMove,
                                      MouseType::Magnify>
                                      mouseType)
    {
        for (auto& child : children) {
            if (const auto childBounds = child->getBounds().toFloat();
                childBounds.contains(mouseEvent.position)) {
                
                const auto childEvent = mouseEvent.withNewPosition(Point<float>{
                    mouseEvent.position.x - childBounds.getX(),
                    mouseEvent.position.y - childBounds.getY()
                });
                std::visit(variantVisitor{
                    [&child, &childEvent](const MouseType::Move&)             { child->mouseMove(childEvent); },
                    [&child, &childEvent](const MouseType::Enter&)            { child->mouseEnter(childEvent); },
                    [&child, &childEvent](const MouseType::Exit&)             { child->mouseExit(childEvent); },
                    [&child, &childEvent](const MouseType::Down&)             { child->mouseDown(childEvent); },
                    [&child, &childEvent](const MouseType::Drag&)             { child->mouseDrag(childEvent); },
                    [&child, &childEvent](const MouseType::Up&)               { child->mouseUp(childEvent); },
                    [&child, &childEvent](const MouseType::DoubleClick&)      { child->mouseDoubleClick(childEvent); },
                    [&child, &childEvent](const MouseType::WheelMove& m)      { child->mouseWheelMove(childEvent, m.wheel); },
                    [&child, &childEvent](const MouseType::Magnify& m)        { child->mouseMagnify(childEvent, m.scaleFactor); }
                }, mouseType);
                
                return;
            }
        }
    }
    template<class... Ts> struct variantVisitor : Ts... { using Ts::operator()...; };   // overloaded call operator
    template<class... Ts> variantVisitor(Ts...) -> variantVisitor<Ts...>;               // arg deduction guide
}; // end OpenGLParentComponent
