/*
  ==============================================================================

    Console.h
    Created: 12 Feb 2019 10:37:53am
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class MainComponent;

//==============================================================================

class Console : public DocumentWindow
{
public:
    //==============================================================================
    
    explicit Console(MainComponent& main_window);
    ~Console() = default;
    void closeButtonPressed() override;

private:
    //==============================================================================
    
    struct Content : public Component
    {
        struct Log : public Logger
        {
            StringArray events;
            static constexpr int history_lines = 40;
            Content& content;

            //==============================================================================

            explicit Log(Content& content);
            ~Log() = default;
            void logMessage (const String& message) override;
            
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Log)
        };
        //==============================================================================

        Log log{ *this };
        TextEditor parent_txt, mouse_txt, compiler_txt;
        Slider compile_rate_sld, period_sld;
        Label compile_rate_lbl, period_lbl;
        static float constexpr row_scalar = 1.f / 40;
        Console& console;
        
        //==============================================================================

        explicit Content(Console& console);
        ~Content();
        void resized() override;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Content)
    };
    MainComponent& main_window;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Console)
};
