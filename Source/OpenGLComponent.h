/*
  ==============================================================================

    OpenGLComponent.h
    Created: 8 Feb 2019 8:34:52am
    Authors:  Ian Caburian
    Thanks to Marius Metzger (Crushed Pixel)
 
  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <variant>

//==============================================================================

class OpenGLRectangle final
{
public:
    //==========================================================================
    
    OpenGLRectangle() {}
    ~OpenGLRectangle() = default;
    
    //==========================================================================
    
    void create()
    {
        static constexpr auto positionsCount = 8;
        static constexpr GLfloat positions[positionsCount] {
            -1.0f, -1.0f,
             1.0f, -1.0f,
             1.0f,  1.0f,
            -1.0f,  1.0f
        };
        static constexpr GLuint elements[elementsCount] {
            0, 1, 2,
            0, 2, 3
        };
        auto* openGLContext = OpenGLContext::getCurrentContext();
        openGLContext->extensions.glGenBuffers(1, &bufferID);
        openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positionsCount,
                                               positions, GL_STATIC_DRAW);
        openGLContext->extensions.glGenBuffers(1, &elementsID);
        openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsID);
        openGLContext->extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elementsCount,
                                              elements, GL_STATIC_DRAW);
    }
    void render()
    {
        auto* openGLContext = OpenGLContext::getCurrentContext();
        openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsID);
        openGLContext->extensions.glVertexAttribPointer (positionAttribID, dimensions, GL_FLOAT, GL_FALSE,
                                                         sizeof(GLfloat) * dimensions, nullptr);
        openGLContext->extensions.glEnableVertexAttribArray (positionAttribID);
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, nullptr);
        openGLContext->extensions.glDisableVertexAttribArray (positionAttribID);
    }
    void delete_vertex_objects()
    {
        auto* openGLContext = OpenGLContext::getCurrentContext();
        openGLContext->extensions.glDeleteBuffers (1, &bufferID);
        openGLContext->extensions.glDeleteBuffers (1, &elementsID);
    }
    
private:
    //==========================================================================
    
    GLuint bufferID{}, elementsID{};
    static constexpr GLint elementsCount = 6, positionAttribID = 0, dimensions = 2;

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

class OpenGLRendererComponent : public OpenGLRenderer, public Component
{
public:
    //==========================================================================
    
    Rectangle<float> getShaderBoundsToFloat() const { return shaderBounds.toFloat(); }
    void setShaderBounds(const int x, const int y, const int w, const int h) { shaderBounds = { x, y, w, h}; }
    
private:
    //==========================================================================
    
    Rectangle<int> shaderBounds;
};

//==============================================================================

class OpenGLChildComponent : public OpenGLRendererComponent
{
public:
    //==========================================================================
    
    struct MouseState
    {
        MouseVariant lastEventType{ MouseType::Up{} };
        Point<float> eventPosition{}, mouseUpPosition{}, mouseDownPosition{}, wheelDelta{};
        OpenGLChildComponent& child;
        float scaleFactor{};
        int eventTime{}, mouseUpTime{}, mouseDownTime{};
        bool isDown{}, isRightClick{}, isToggled{};
        
        MouseState(OpenGLChildComponent& child) : child{ child } {}
        void captureEvent(const MouseEvent& mouseEvent, MouseVariant&& eventType)
        {
            lastEventType       = eventType;
            eventPosition       = mouseEvent.position;
            mouseDownPosition   = mouseEvent.mouseDownPosition;
            eventTime           = mouseEvent.eventTime.toMilliseconds() % std::numeric_limits<int>::max();
            mouseDownTime       = mouseEvent.mouseDownTime.toMilliseconds() % std::numeric_limits<int>::max();
            if (isDown) {
                mouseUpPosition = mouseEvent.position;
                mouseUpTime     = eventTime;
            }
            std::visit([this](const auto& m) {
                using T = std::decay_t<decltype(m)>;
                if constexpr (std::is_same_v<T, MouseType::WheelMove>) {
                    wheelDelta = { m.wheel.deltaX, m.wheel.deltaY };
                }
                else if constexpr (std::is_same_v<T, MouseType::Magnify>) {
                    scaleFactor = m.scaleFactor;
                }
            }, eventType);
            
            logMouseCall(mouseEvent, std::move(eventType));
        }
        void logMouseCall(const MouseEvent& mouseEvent, MouseVariant&& eventType)
        {
            auto strf8 = [](const float f) { return String::formatted("% .2f", f).paddedLeft(' ', 8); };
            auto strf1 = [](const float f) { return String::formatted("% .2f", f); };
            auto s = String{ "m " };
            s <<   "      componentID: " << child.getComponentID()
              << "\n           bounds: " << child.getShaderBoundsToFloat().toNearestInt().toString()
              << "\n    mousePosition:"  << strf8(eventPosition.x) << " "
                                         << strf8(eventPosition.y)
              << "\n  mouseUpPosition:"  << strf8(mouseUpPosition.x) << " "
                                         << strf8(mouseUpPosition.y)
              << "\nmouseDownPosition:"  << strf8(mouseDownPosition.x) << " "
                                         << strf8(mouseDownPosition.y)
              << "\n           isDown? " << (isDown ? "DOWN" : "UP  ")
              << "\n     isRightClick? " << (isRightClick ? "RIGHT" : "LEFT ")
              << "\n        isToggled? " << (isToggled ? "ON " : "OFF")
              << "\n      mouseUpTime: " << mouseUpTime
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

    //==========================================================================
    
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

    //==========================================================================

    MouseState copyMouseState() const { return mouseState; }

private:
    //==========================================================================
    
    MouseState mouseState{ *this };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLChildComponent)
};

//==============================================================================

class OpenGLParentComponent : public OpenGLRendererComponent
{
public:
    //==========================================================================

    explicit OpenGLParentComponent() = default;
    ~OpenGLParentComponent() = default;
    void newOpenGLContextCreated() override final
    {
        newOpenGLContextCreatedParent();
        visitChildren([](auto& child) { child.newOpenGLContextCreated(); });
        checkContextCreation();
    }
    void renderOpenGL() override final
    {
        const auto renderingScale = OpenGLContext::getCurrentContext()->getRenderingScale();
        renderOpenGLParent();
        auto* topLevelComponent = getTopLevelComponent();
        const auto positionToTopLevel = getLocalPoint(topLevelComponent, Point<int>{});
        const auto topLevelHeight = topLevelComponent->getHeight();
        visitChildren([&](auto& child) {
            glEnable(GL_SCISSOR_TEST);
            clipDrawArea(child, positionToTopLevel, renderingScale, topLevelHeight);
            child.renderOpenGL();
            glDisable(GL_SCISSOR_TEST);
        });
        resetBuffers();
    }
    void openGLContextClosing() override final
    {
        visitChildren([](auto& child) { child.openGLContextClosing(); });
        openGLContextClosingParent();
    }

protected:
    //==========================================================================

    virtual void newOpenGLContextCreatedParent() {}
    virtual void checkContextCreation() {}
    virtual void renderOpenGLParent() {}
    virtual void openGLContextClosingParent() {}
    virtual void resetBuffers() {}
    virtual void clipDrawArea(OpenGLRendererComponent& child, const Point<int>& positionToTopLevel,
                              const double renderingScale, const int topLevelHeight)
    {
        const auto x = roundToInt((std::abs(positionToTopLevel.x) + child.getX()) * renderingScale);
        const auto y = roundToInt((topLevelHeight - (std::abs(positionToTopLevel.y) + child.getBottom())) * renderingScale);
        const auto w = roundToInt(child.getWidth() * renderingScale);
        const auto h = roundToInt(child.getHeight() * renderingScale);
        child.setShaderBounds(x, y, w, h);
        glViewport(x, y, w, h);
        glScissor(x, y, w, h);
    }

    //==========================================================================

    void addOpenGLRendererComponent(OpenGLRendererComponent* child)
    {
        addAndMakeVisible(*children.emplace_back(child));
    }
    void removeOpenGLRendererComponent(OpenGLRendererComponent* child)
    {
        if (const auto result = std::find(children.begin(), children.end(), child);
            result != children.end()) {
            removeChildComponent(child);
            children.erase(result);
        }
    }
    std::size_t getNumOpenGLRendererComponents() const { return children.size(); }
    void visitChildren(std::function<void(OpenGLRendererComponent&)> f) { for (auto& c : children) f(*c); }
    
private:
    //==========================================================================
    
    std::vector<OpenGLRendererComponent*> children;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLParentComponent)
};
