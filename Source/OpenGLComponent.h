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

//==============================================================================

class OpenGLRectangle final
{
    using GL = juce::OpenGLExtensionFunctions;
public:
    //==============================================================================
    
    OpenGLRectangle() {}
    ~OpenGLRectangle() = default;
    
    //==============================================================================
    
    void create()
    {
        constexpr auto positionsCount = 8;
        const GLfloat positions[positionsCount] {
            -1.0f, -1.0f,
             1.0f, -1.0f,
             1.0f,  1.0f,
            -1.0f,  1.0f
        };
        const GLuint elements[elementsCount] {
            0, 1, 2,
            0, 2, 3
        };
        GL::glGenVertexArrays(1, &arrayID);
        GL::glBindVertexArray(arrayID);
        
        GL::glGenBuffers(1, &bufferID);
        GL::glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        GL::glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positionsCount, positions, GL_STATIC_DRAW);
        
        const auto positionAttribID = 0, dimensions = 2;
        GL::glEnableVertexAttribArray(positionAttribID);
        GL::glVertexAttribPointer(positionAttribID, dimensions, GL_FLOAT, GL_FALSE,
                                  sizeof(GLfloat) * dimensions, 0);
        
        GL::glGenBuffers(1, &elementsID);
        GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsID);
        GL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elementsCount, elements, GL_STATIC_DRAW);
    }
    void render()
    {
        GL::glBindVertexArray(arrayID);
        GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsID);
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, nullptr);
    }
    void delete_vertex_objects()
    {
        GL::glDeleteVertexArrays(1, &arrayID);
        GL::glDeleteBuffers(1, &bufferID);
        GL::glDeleteBuffers(1, &elementsID);
    }
    
private:
    //==============================================================================
    
    GLuint arrayID{}, bufferID{}, elementsID{};
    static constexpr int elementsCount = 6;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLRectangle)
};

//==============================================================================

struct MouseType
{
    struct Move{};
    struct Enter{};
    struct Exit{};
    struct Down{};
    struct Drag{};
    struct Up{};
    struct DoubleClick{};
    struct WheelMove{ MouseWheelDetails wheel; };
    struct Magnify{ float scaleFactor; };
};

using MouseVariant = std::variant<MouseType::Move,
                                  MouseType::Enter,
                                  MouseType::Exit,
                                  MouseType::Down,
                                  MouseType::Drag,
                                  MouseType::Up,
                                  MouseType::DoubleClick,
                                  MouseType::WheelMove,
                                  MouseType::Magnify>;

//==============================================================================

template<class... Ts> struct VariantVisitor : Ts... { using Ts::operator()...; };   // overloaded call operator
template<class... Ts> VariantVisitor(Ts...) -> VariantVisitor<Ts...>;               // arg deduction guide

//==============================================================================

class OpenGLChildComponent : public MouseListener, public OpenGLRenderer
{
public:
    //==============================================================================
    
    struct MouseState
    {
        MouseVariant lastEventType{ MouseType::Up{} };
        Point<float> mousePosition{}, mouseDownPosition{};
        float eventTime{}, mouseDownTime{};
        bool isDown{}, isRightClick{}, isToggled{};
        
        void captureEvent(const MouseEvent& mouseEvent, MouseVariant&& eventType)
        {
            lastEventType = eventType;
            mousePosition = mouseEvent.position;
            mouseDownPosition = mouseEvent.mouseDownPosition;
            eventTime       = std::fmodf(static_cast<double>(mouseEvent.eventTime.toMilliseconds()),
                                         static_cast<double>(std::numeric_limits<float>::max()));
            mouseDownTime   = std::fmodf(static_cast<double>(mouseEvent.mouseDownTime.toMilliseconds()),
                                         static_cast<double>(std::numeric_limits<float>::max()));
        }
    };

    //==============================================================================
    
    explicit OpenGLChildComponent() = default;
    ~OpenGLChildComponent() = default;
    void mouseMove(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Move{});
        DBG("mouseMove() | #" << componentID << " eventPosition: " << mouseEvent.position.toString()); // TODO to log console
    }
    void mouseEnter(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Enter{});
        DBG("mouseEnter() | #" << componentID << " eventPosition: " << mouseEvent.position.toString()); // TODO to log console
    }
    void mouseExit(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Exit{});
        DBG("mouseExit() | #" << componentID << " eventPosition: " << mouseEvent.position.toString()); // TODO to log console
    }
    void mouseDown(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Down{});
        mouseState.isToggled = ! mouseState.isToggled;
        mouseState.isDown = true;
        mouseState.isRightClick = mouseEvent.mods.isRightButtonDown();
        DBG("mouseDown() | #" << componentID << " eventPosition: " << mouseEvent.position.toString()); // TODO to log console
    }
    void mouseDrag(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Drag{});
        DBG("mouseDrag() | #" << componentID << " eventPosition: " << mouseEvent.position.toString()); // TODO to log console
    }
    void mouseUp(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Up{});
        mouseState.isDown = false;
        DBG("mouseUp() | #" << componentID << " eventPosition: " << mouseEvent.position.toString());
    }
    void mouseDoubleClick(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::DoubleClick{});
        DBG("mouseDoubleClick() | #" << componentID << " eventPosition: " << mouseEvent.position.toString()); // TODO to log console

    }
    void mouseWheelMove(const MouseEvent& mouseEvent, const MouseWheelDetails& wheel) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::WheelMove{ wheel });
        DBG("mouseWheelMove() | #" << componentID << " eventPosition: " << mouseEvent.position.toString()); // TODO to log console

    }
    void mouseMagnify(const MouseEvent& mouseEvent, float scaleFactor) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Magnify{ scaleFactor });
        DBG("mouseMagnify() | #" << componentID << " eventPosition: " << mouseEvent.position.toString()); // TODO to log console
    }

    //==============================================================================

    void            setComponentID(const String& newID)             { componentID = newID; }
    void            setBounds(const Rectangle<int>& new_bounds)     { bounds = new_bounds; }
    MouseState      copyMouseState() const                          { return mouseState; }
    const String&   getComponentID() const                          { return componentID; }
    Rectangle<int>  getBounds() const                               { return bounds; }
    int             getX() const                                    { return bounds.getX(); }
    int             getBottom() const                               { return bounds.getBottom(); }
    int             getWidth() const                                { return bounds.getWidth(); }
    int             getHeight() const                               { return bounds.getHeight(); }

private:
    //==============================================================================
    
    MouseState mouseState{};
    String componentID{};
    Rectangle<int> bounds{};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLChildComponent)
};

//==============================================================================

class OpenGLParentComponent : public Component, public OpenGLRenderer
{
public:
    //==============================================================================
    explicit OpenGLParentComponent()
    {
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
    //==============================================================================

    OpenGLContext openGLContext;
    
    //==============================================================================

    virtual void newOpenGLContextCreatedParent() {}
    virtual void renderOpenGLParent() {}
    virtual void openGLContextClosingParent() {}
    
    //==============================================================================

    void addOpenGLChildComponent(const std::shared_ptr<OpenGLChildComponent> new_child) { children.push_back(new_child); }
    OpenGLChildComponent* removeOpenGLChildComponent(int childIndexToRemove) { return children.erase(children.begin() + childIndexToRemove)->get(); }
    void visitChildren(std::function<void(OpenGLChildComponent&)> f) { for (auto& child : children) f(*child); }
    std::size_t getNumOpenGLChildComponents() const { return children.size(); }
    float getRenderingScale() const { return renderingScale; }
    
private:
    //==============================================================================

    std::vector<std::shared_ptr<OpenGLChildComponent>> children;
    float renderingScale{};
    
    //==============================================================================

    void newOpenGLContextCreated() override final
    {
        newOpenGLContextCreatedParent();
        visitChildren([](auto& child) { child.newOpenGLContextCreated(); });
    }
    void renderOpenGL() override final
    {
        renderingScale = openGLContext.getRenderingScale();

        glEnable(GL_SCISSOR_TEST); // This needs to be called here, and has no effect in the initialization function for some reason
        
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
    void clipDrawArea(const Rectangle<int>& area)
    {
        const auto x = area.getX() * renderingScale;
        const auto y = (getHeight() - area.getBottom()) * renderingScale;
        const auto w = area.getWidth() * renderingScale;
        const auto h = area.getHeight() * renderingScale;
        glViewport(x, y, w, h);
        glScissor(x, y, w, h);
    }
    void mouseCall(const MouseEvent& mouseEvent, const MouseVariant mouseVariant)
    {
        for (auto& child : children) {
            if (const auto childBounds = child->getBounds().toFloat();
                childBounds.contains(mouseEvent.mouseDownPosition)) {
                
                const auto childEvent = mouseEvent.withNewPosition(Point<float>{
                    mouseEvent.position.x - childBounds.getX(),
                    mouseEvent.position.y - childBounds.getY()
                });
                std::visit(VariantVisitor{
                    [&child, &childEvent](const MouseType::Move&)             { child->mouseMove(childEvent); },
                    [&child, &childEvent](const MouseType::Enter&)            { child->mouseEnter(childEvent); },
                    [&child, &childEvent](const MouseType::Exit&)             { child->mouseExit(childEvent); },
                    [&child, &childEvent](const MouseType::Down&)             { child->mouseDown(childEvent); },
                    [&child, &childEvent](const MouseType::Drag&)             { child->mouseDrag(childEvent); },
                    [&child, &childEvent](const MouseType::Up&)               { child->mouseUp(childEvent); },
                    [&child, &childEvent](const MouseType::DoubleClick&)      { child->mouseDoubleClick(childEvent); },
                    [&child, &childEvent](const MouseType::WheelMove& m)      { child->mouseWheelMove(childEvent, m.wheel); },
                    [&child, &childEvent](const MouseType::Magnify& m)        { child->mouseMagnify(childEvent, m.scaleFactor); }
                }, mouseVariant);
                
                return;
            }
        }
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLParentComponent)
};
