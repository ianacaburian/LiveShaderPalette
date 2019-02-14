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

class OpenGLRendererComponent : public OpenGLRenderer, public Component {};

//==============================================================================

class OpenGLChildComponent : public OpenGLRendererComponent
{
public:
    //==============================================================================
    
    struct MouseState
    {
        MouseVariant lastEventType{ MouseType::Up{} };
        Point<float> mousePosition{}, mouseDownPosition{};
        OpenGLChildComponent& child;
        int eventTime{}, mouseDownTime{};
        bool isDown{}, isRightClick{}, isToggled{};
        
        MouseState(OpenGLChildComponent& child) : child{ child } {}
        void captureEvent(const MouseEvent& mouseEvent, MouseVariant&& eventType)
        {
            lastEventType = eventType;
            mousePosition = mouseEvent.position;
            mouseDownPosition = mouseEvent.mouseDownPosition;
            eventTime       = mouseEvent.eventTime.toMilliseconds() % std::numeric_limits<int>::max();
            mouseDownTime   = mouseEvent.mouseDownTime.toMilliseconds() % std::numeric_limits<int>::max();
            
            logMouseCall(mouseEvent, std::move(eventType));
        }
        void logMouseCall(const MouseEvent& mouseEvent, MouseVariant&& eventType)
        {
            auto strf8 = [](const float f) { return String::formatted("% .2f", f).paddedLeft(' ', 8); };
            auto strf1 = [](const float f) { return String::formatted("% .2f", f); };
            auto s = String{ "m " };
            s <<   "      componentID: " << child.getComponentID()
              << "\n    mousePosition:"  << strf8(mousePosition.x) << " "
                                         << strf8(mousePosition.y)
              << "\nmouseDownPosition:"  << strf8(mouseDownPosition.x) << " "
                                         << strf8(mouseDownPosition.y)
              << "\n           isDown? " << (isDown ? "DOWN" : "UP  ")
              << "\n     isRightClick? " << (isRightClick ? "RIGHT" : "LEFT ")
              << "\n        isToggled? " << (isToggled ? "ON " : "OFF")
              << "\n    mouseDownTime: " << mouseDownTime
              << "\n        eventTime: " << eventTime << " |"
              << " Type: ";
            std::visit(Overloader{
                [&s](const MouseType::Move&)        { s << "Move"; },
                [&s](const MouseType::Enter&)       { s << "Enter"; },
                [&s](const MouseType::Exit&)        { s << "Exit"; },
                [&s](const MouseType::Down&)        { s << "Down"; },
                [&s](const MouseType::Drag&)        { s << "Drag"; },
                [&s](const MouseType::Up&)          { s << "Up"; },
                [&s](const MouseType::DoubleClick&) { s << "DoubleClick"; },
                [&s, &strf1](const MouseType::WheelMove& m) {
                    s << "WheelMove.delta:" << strf1(m.wheel.deltaX) << " "
                                            << strf1(m.wheel.deltaY);
                },
                [&s, &strf1](const MouseType::Magnify& m) {
                    s << "Magnify.scaleFactor:" << strf1(m.scaleFactor);
                }
            }, eventType);
            Logger::writeToLog(s);
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
        mouseState.isToggled = ! mouseState.isToggled;
        mouseState.isDown = true;
        mouseState.isRightClick = mouseEvent.mods.isRightButtonDown();
        mouseState.captureEvent(mouseEvent, MouseType::Down{});
    }
    void mouseDrag(const MouseEvent& mouseEvent) override
    {
        mouseState.captureEvent(mouseEvent, MouseType::Drag{});
    }
    void mouseUp(const MouseEvent& mouseEvent) override
    {
        mouseState.isDown = false;
        mouseState.captureEvent(mouseEvent, MouseType::Up{});
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

    MouseState      copyMouseState() const                          { return mouseState; }

private:
    //==============================================================================
    
    MouseState mouseState{ *this };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLChildComponent)
};

//==============================================================================

class OpenGLParentComponent : public OpenGLRendererComponent
{
public:
    //==============================================================================

    explicit OpenGLParentComponent() = default;
    ~OpenGLParentComponent() = default;
    void newOpenGLContextCreated() override final
    {
        newOpenGLContextCreatedParent();
        visitChildren([](auto& child) { child.newOpenGLContextCreated(); });
    }
    void renderOpenGL() override final
    {
        glEnable(GL_SCISSOR_TEST);
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

protected:
    //==============================================================================

    virtual void newOpenGLContextCreatedParent() {}
    virtual void renderOpenGLParent() {}
    virtual void openGLContextClosingParent() {}
    virtual float getRenderingScale() const = 0;
    virtual void clipDrawArea(const Rectangle<int>& child)
    {
        const auto renderingScale = getRenderingScale();
        auto* appWindow = getTopLevelComponent();
        const auto appPosition = getLocalPoint(appWindow, Point<int>{ getX(), getY() });
        const auto x = (appPosition.x + child.getX()) * renderingScale;
        const auto y = (appWindow->getHeight() - (appPosition.y + child.getBottom())) * renderingScale;
        const auto w = child.getWidth() * renderingScale;
        const auto h = child.getHeight() * renderingScale;
        glViewport(x, y, w, h);
        glScissor(x, y, w, h);
    }

    //==============================================================================

    void addOpenGLRendererComponent(OpenGLRendererComponent* child)
    {
        addAndMakeVisible(*children.emplace_back(std::unique_ptr<OpenGLRendererComponent>{ child }));
    }
    void removeOpenGLRendererComponent(OpenGLRendererComponent* child)
    {
        if (const auto result = std::find_if(children.begin(), children.end(),
                                             [&](const auto& c) { return c.get() == child; });
            result != children.end()) {
            children.erase(result);
        }
    }
    std::size_t getNumOpenGLRendererComponents() const { return children.size(); }
    void visitChildren(std::function<void(OpenGLRendererComponent&)> f) { for (auto& c : children) f(*c); }
    
private:
    //==============================================================================
    
    std::vector<std::unique_ptr<OpenGLRendererComponent>> children;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLParentComponent)
};

//==============================================================================

class OpenGLTopLevelComponent : public OpenGLParentComponent
{
public:
    //==============================================================================
    
    explicit OpenGLTopLevelComponent()
    {
        openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
        openGLContext.setContinuousRepainting(true);
        openGLContext.setComponentPaintingEnabled(true);
        openGLContext.setMultisamplingEnabled(true);                            // Eliminates flickering, no idea how or why.
        openGLContext.setRenderer(this);
        openGLContext.attachTo(*this);        
    }
    float getRenderingScale() const override { return openGLContext.getRenderingScale(); }
    
protected:
    //==============================================================================

    OpenGLContext openGLContext;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLTopLevelComponent)
};