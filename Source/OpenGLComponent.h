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

template<class... Ts> struct Overloader : Ts... { using Ts::operator()...; };
template<class... Ts> Overloader(Ts...) -> Overloader<Ts...>;

//==============================================================================

class OpenGLChildComponent : public MouseListener, public OpenGLRenderer
{
public:
    //==============================================================================
    
    struct MouseState
    {
        MouseVariant lastEventType{ MouseType::Up{} };
        Point<float> mousePosition{}, mouseDownPosition{};
        int eventTime{}, mouseDownTime{};
        bool isDown{}, isRightClick{}, isToggled{};
        
        void captureEvent(const MouseEvent& mouseEvent, MouseVariant&& eventType)
        {
            lastEventType = eventType;
            mousePosition = mouseEvent.position;
            mouseDownPosition = mouseEvent.mouseDownPosition;
            eventTime       = mouseEvent.eventTime.toMilliseconds() % std::numeric_limits<int>::max();
            mouseDownTime   = mouseEvent.mouseDownTime.toMilliseconds() % std::numeric_limits<int>::max();
        }
    };

    //==============================================================================
    
    explicit OpenGLChildComponent() = default;
    ~OpenGLChildComponent() = default;
    void mouseMove(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Move{});
    }
    void mouseEnter(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Enter{});
    }
    void mouseExit(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Exit{});
    }
    void mouseDown(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Down{});
        mouseState.isToggled = ! mouseState.isToggled;
        mouseState.isDown = true;
        mouseState.isRightClick = mouseEvent.mods.isRightButtonDown();
    }
    void mouseDrag(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Drag{});
    }
    void mouseUp(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Up{});
        mouseState.isDown = false;
    }
    void mouseDoubleClick(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::DoubleClick{});
    }
    void mouseWheelMove(const MouseEvent& mouseEvent, const MouseWheelDetails& wheel) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::WheelMove{ wheel });
    }
    void mouseMagnify(const MouseEvent& mouseEvent, float scaleFactor) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Magnify{ scaleFactor });
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
    float getRenderingScale() const { return renderingScale; }
    bool isLogging() const { return logging; }
    
protected:
    //==============================================================================

    OpenGLContext openGLContext;
    bool logging{};
    
    //==============================================================================

    virtual void newOpenGLContextCreatedParent() {}
    virtual void renderOpenGLParent() {}
    virtual void openGLContextClosingParent() {}
    
    //==============================================================================

    void addOpenGLChildComponent(const std::shared_ptr<OpenGLChildComponent> new_child)
    {
        children.push_back(new_child);
    }
    OpenGLChildComponent* removeOpenGLChildComponent(int childIndexToRemove)
    {
        return children.erase(children.begin() + childIndexToRemove)->get();
    }
    void visitChildren(std::function<void(OpenGLChildComponent&)> f)
    {
        for (auto& child : children) {
            f(*child);
        }
    }
    std::size_t getNumOpenGLChildComponents() const { return children.size(); }
    
private:
    //==============================================================================

    std::vector<std::shared_ptr<OpenGLChildComponent>> children;
    MemoryOutputStream memoryOutputStream;
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
    void mouseCall(const MouseEvent& mouseEvent, MouseVariant&& mouseVariant)
    {
        for (auto& child : children) {
            if (const auto childBounds = child->getBounds().toFloat();
                childBounds.contains(mouseEvent.mouseDownPosition)) {
                
                const auto childEvent = mouseEvent.withNewPosition(Point<float>{
                    mouseEvent.position.x - childBounds.getX(),
                    mouseEvent.position.y - childBounds.getY()
                });
                std::visit(Overloader{
                    [&child, &childEvent](const MouseType::Move&) { child->mouseMove(childEvent); },
                    [&child, &childEvent](const MouseType::Enter&) { child->mouseEnter(childEvent); },
                    [&child, &childEvent](const MouseType::Exit&) { child->mouseExit(childEvent); },
                    [&child, &childEvent](const MouseType::Down&) { child->mouseDown(childEvent); },
                    [&child, &childEvent](const MouseType::Drag&) { child->mouseDrag(childEvent); },
                    [&child, &childEvent](const MouseType::Up&) { child->mouseUp(childEvent); },
                    [&child, &childEvent](const MouseType::DoubleClick&) { child->mouseDoubleClick(childEvent); },
                    [&child, &childEvent](const MouseType::WheelMove& m) { child->mouseWheelMove(childEvent, m.wheel); },
                    [&child, &childEvent](const MouseType::Magnify& m) { child->mouseMagnify(childEvent, m.scaleFactor); }
                }, mouseVariant);
                
                if (logging) {
                    logMouseCall(*child, childEvent, std::move(mouseVariant));
                }
                return;
            }
        }
    }
    void logMouseCall(const OpenGLChildComponent& child, const MouseEvent& mouseEvent, MouseVariant&& eventType)
    {
        const auto mouseState = child.copyMouseState();
        auto strf = [](const float f) { return String::formatted("% .2f", f).paddedLeft(' ', 8); };
        auto s = String{ "m" }, options = String{};
        s << " Component ID: " << child.getComponentID()
          << "\nPosition: (" << strf(mouseState.mousePosition.x) << ","
          << strf(mouseState.mousePosition.y) << " ) |"
          << " Mouse Down Position: (" << strf(mouseState.mouseDownPosition.x) << ","
          << strf(mouseState.mouseDownPosition.y) << " )"
          << "\nMouse is: " << (mouseState.isDown ? "DOWN" : "UP  ") << " |"
          << " Button: " << (mouseState.isRightClick ? "RIGHT" : "LEFT ") << " |"
          << " Toggle: " << (mouseState.isToggled ? "ON " : "OFF") << "\nType: ";
        
        std::visit(Overloader{
            [&s](const MouseType::Move&) { s << "Move"; },
            [&s](const MouseType::Enter&) { s << "Enter"; },
            [&s](const MouseType::Exit&) { s << "Exit"; },
            [&s](const MouseType::Down&) { s << "Down"; },
            [&s](const MouseType::Drag&) { s << "Drag"; },
            [&s](const MouseType::Up&) { s << "Up"; },
            [&s](const MouseType::DoubleClick&) { s << "DoubleClick"; },
            [&s, &options, &strf](const MouseType::WheelMove& m) {
                s << "WheelMove";
                options << " Delta: ( " << strf(m.wheel.deltaX) << "," << strf(m.wheel.deltaY) << " )";
            },
            [&s, &options, &strf](const MouseType::Magnify& m) {
                s << "Magnify";
                options << " Scale Factor:" << strf(m.scaleFactor);
            }
        }, eventType);
        
        s << " | Event Time: " << mouseState.eventTime << " |"
          << " Mouse Down Time: " << mouseState.mouseDownTime << " |"
          << options;

        Logger::writeToLog(s);
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLParentComponent)
};
