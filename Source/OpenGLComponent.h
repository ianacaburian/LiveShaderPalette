/*
  ==============================================================================

    OpenGLComponent.h
    Created: 8 Feb 2019 8:34:52am
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <variant>
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
        openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
        openGLContext.setRenderer(this);
        openGLContext.setContinuousRepainting(true);
        openGLContext.attachTo(*this);
    }
    ~OpenGLParentComponent()
    {
        openGLContext.detach();
    }
    void mouseMove(const MouseEvent& m) override final { MouseCall{ *this, m, MouseCall::MouseMove{} }; }
    void mouseEnter(const MouseEvent& m) override final { MouseCall{ *this, m, MouseCall::MouseEnter{} }; }
    void mouseExit(const MouseEvent& m) override final { MouseCall{ *this, m, MouseCall::MouseExit{} }; }
    void mouseDown(const MouseEvent& m) override final { MouseCall{ *this, m, MouseCall::MouseDown{} }; }
    void mouseDrag(const MouseEvent& m) override final { MouseCall{ *this, m, MouseCall::MouseDrag{} }; }
    void mouseUp(const MouseEvent& m) override final { MouseCall{ *this, m, MouseCall::MouseUp{} }; }
    void mouseDoubleClick(const MouseEvent& m) override final { MouseCall{ *this, m, MouseCall::MouseDoubleClick{} }; }
    void mouseWheelMove(const MouseEvent& m, const MouseWheelDetails& w) override final { MouseCall{ *this, m, MouseCall::MouseWheelMove{ w }}; }
    void mouseMagnify(const MouseEvent& m, float s) override final { MouseCall{ *this, m, MouseCall::MouseMagnify{ s } }; }

protected:
    OpenGLContext openGLContext;
    
    virtual void newOpenGLContextCreatedParent() {}
    virtual void renderOpenGLParent() {}
    virtual void openGLContextClosingParent() {}
    
    void addChildComponent(const std::shared_ptr<OpenGLChildComponent> new_child) { children.push_back(new_child); }
    void visitChildren(std::function<void(OpenGLChildComponent&)> f) { for (auto& child : children) f(*child); }
    float getRenderingScale() const { return renderingScale; }
    
private:
    std::vector<std::shared_ptr<OpenGLChildComponent>> children;
    float renderingScale{};
    
    void newOpenGLContextCreated() override final
    {
        renderingScale = openGLContext.getRenderingScale();
        newOpenGLContextCreatedParent();
        visitChildren([](auto& child) { child.newOpenGLContextCreated(); });
    }
    void renderOpenGL() override final
    {
        glEnable(GL_SCISSOR_TEST); // This needs to be called here, and has no effect in the initialization function for some reason
        
        renderOpenGLParent();
        
        visitChildren([this](auto& child) {
            const auto x = child.getX() * renderingScale;
            const auto y = (getHeight() - child.getBottom()) * renderingScale;
            const auto w = child.getWidth() * renderingScale;
            const auto h = child.getHeight() * renderingScale;
            glViewport(x, y, w, h);
            glScissor(x, y, w, h);
            child.renderOpenGL();
        });
    }
    void openGLContextClosing() override final
    {
        visitChildren([](auto& child) { child.openGLContextClosing(); });
        openGLContextClosingParent();
    }
    //==========================================================================
    class MouseCall
    {
    public:
        struct MouseMove{};
        struct MouseEnter{};
        struct MouseExit{};
        struct MouseDown{};
        struct MouseDrag{};
        struct MouseUp{};
        struct MouseDoubleClick{};
        struct MouseWheelMove{ const MouseWheelDetails& wheel; };
        struct MouseMagnify{ float scaleFactor; };
        
        MouseCall(OpenGLParentComponent& parent, const MouseEvent& mouseEvent,
                  const std::variant<MouseMove, MouseEnter, MouseExit, MouseDown, MouseDrag,
                  MouseUp, MouseDoubleClick, MouseWheelMove, MouseMagnify> type)
        {
            parent.visitChildren([&](auto& child) {
                if (const auto bounds = child.getBounds().toFloat();
                    bounds.contains(mouseEvent.position)) {
                    
                    const auto relativeEvent = mouseEvent.withNewPosition(Point<float>{
                        mouseEvent.position.x - bounds.getX(),
                        mouseEvent.position.y - bounds.getY()
                    });
                    std::visit(variantVisitor{
                        [&](const MouseMove&)            { child.mouseMove(relativeEvent); },
                        [&](const MouseEnter&)           { child.mouseEnter(relativeEvent); },
                        [&](const MouseExit&)            { child.mouseExit(relativeEvent); },
                        [&](const MouseDown&)            { child.mouseDown(relativeEvent); },
                        [&](const MouseDrag&)            { child.mouseDrag(relativeEvent); },
                        [&](const MouseUp&)              { child.mouseUp(relativeEvent); },
                        [&](const MouseDoubleClick&)     { child.mouseDoubleClick(relativeEvent); },
                        [&](const MouseWheelMove& mwm)   { child.mouseWheelMove(relativeEvent, mwm.wheel); },
                        [&](const MouseMagnify& mm)      { child.mouseMagnify(relativeEvent, mm.scaleFactor); }
                    }, type);
                }
            });
        }
    private:
        template<class... Ts> struct variantVisitor : Ts... { using Ts::operator()...; };   // overloaded call operator
        template<class... Ts> variantVisitor(Ts...) -> variantVisitor<Ts...>;               // variant deduction guide
    };
};
