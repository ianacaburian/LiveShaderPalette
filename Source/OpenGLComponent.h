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

//==============================================================================

class OpenGLRectangle final
{
    using GL = juce::OpenGLExtensionFunctions;
public:
    OpenGLRectangle() {}
    ~OpenGLRectangle() = default;
    void create()
    {
        constexpr auto positions_count = 8;
        const GLfloat positions[positions_count] {
            -1.0f, -1.0f,
             1.0f, -1.0f,
             1.0f,  1.0f,
            -1.0f,  1.0f
        };
        const GLuint elements[elements_count] {
            0, 1, 2,
            0, 2, 3
        };
        GL::glGenVertexArrays(1, &vertex_arr_ID);
        GL::glBindVertexArray(vertex_arr_ID);
        
        GL::glGenBuffers(1, &vertex_buff_ID);
        GL::glBindBuffer(GL_ARRAY_BUFFER, vertex_buff_ID);
        GL::glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positions_count, positions, GL_STATIC_DRAW);
        
        const auto position_attrib_id = 0, dimensions = 2;
        GL::glEnableVertexAttribArray(position_attrib_id);
        GL::glVertexAttribPointer(position_attrib_id, dimensions, GL_FLOAT, GL_FALSE,
                                  sizeof(GLfloat) * dimensions, (const void*)0);
        
        GL::glGenBuffers(1, &index_buff_ID);
        GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
        GL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements_count, elements, GL_STATIC_DRAW);
    }
    void render()
    {
        GL::glBindVertexArray(vertex_arr_ID);
        GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
        glDrawElements(GL_TRIANGLES, elements_count, GL_UNSIGNED_INT, nullptr);
    }
    void delete_vertex_objects()
    {
        GL::glDeleteVertexArrays(1, &vertex_arr_ID);
        GL::glDeleteBuffers(1, &vertex_buff_ID);
        GL::glDeleteBuffers(1, &index_buff_ID);
    }
private:
    static constexpr int elements_count = 6;
    GLuint vertex_arr_ID{}, vertex_buff_ID{}, index_buff_ID{};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLRectangle)
};

//==============================================================================

class OpenGLChildComponent : public MouseListener, public OpenGLRenderer
{
public:
    explicit OpenGLChildComponent() = default;
    ~OpenGLChildComponent() = default;
    Rectangle<int> getBounds() const { return bounds; }
    int getX() const { return bounds.getX(); }
    int getBottom() const { return bounds.getBottom(); }
    int getWidth() const { return bounds.getWidth(); }
    int getHeight() const { return bounds.getHeight(); }
    void setBounds(const Rectangle<int>& new_bounds) { bounds = new_bounds; }
private:
    Rectangle<int> bounds;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLChildComponent)
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
    struct WheelMove{ const MouseWheelDetails& wheel; };
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

class OpenGLParentComponent : public Component, public OpenGLRenderer
{
public:
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
    virtual std::optional<Rectangle<int>> getParentClippedDrawArea() = 0;
    
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
    void mouseCall(const MouseEvent& mouseEvent, const MouseVariant mouseVariant)
    {
        for (auto& child : children) {
            if (const auto childBounds = child->getBounds().toFloat();
                childBounds.contains(mouseEvent.position)) {
                
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
