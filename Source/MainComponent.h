/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "OpenGLComponent.h"
class LiveShaderPanel;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public OpenGLParentComponent, public FileDragAndDropTarget, private Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();
    //==============================================================================
    void resized() override;
    void paint(Graphics& g) override;
    void renderOpenGLParent() override;
    std::optional<Rectangle<int>> getParentClippedDrawArea() override;

    //==============================================================================

    /** Callback to check whether this target is interested in the set of files being offered.
     
     Note that this will be called repeatedly when the user is dragging the mouse around over your
     component, so don't do anything time-consuming in here, like opening the files to have a look
     inside them!
     
     @param files        the set of (absolute) pathnames of the files that the user is dragging
     @returns            true if this component wants to receive the other callbacks regarging this
     type of object; if it returns false, no other callbacks will be made.
     */
    bool isInterestedInFileDrag (const StringArray& files) override;
    
    /** Callback to indicate that the user has dropped the files onto this component.
     
     When the user drops the files, this get called, and you can use the files in whatever
     way is appropriate.
     
     Note that after this is called, the fileDragExit method may not be called, so you should
     clean up in here if there's anything you need to do when the drag finishes.
     
     @param files        the set of (absolute) pathnames of the files that the user is dragging
     @param x            the mouse x position, relative to this component
     @param y            the mouse y position, relative to this component
     */
    void filesDropped (const StringArray& files, int x, int y) override;
    
    //==============================================================================

    float get_rendering_scale() const;
    float get_sin_time() const;
    
private:
    //==============================================================================
    std::vector<std::shared_ptr<LiveShaderPanel>> panels;
    Rectangle<int> toolbar_bounds;
    TextButton live_compile    { "Live Compile" };
    Point<int> screen_resolution { 400, 300 };
    double prev_time{}, ms_frame{};
    float sin_time{};
    int frame_count{}, compile_interval_ms = 2000;
    
    //==============================================================================

    void timerCallback() override;
    void init_buttons();
    void recompile_shaders();
    void print_log();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
