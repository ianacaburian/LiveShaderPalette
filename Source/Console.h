/*
  ==============================================================================

    Console.h
    Created: 12 Feb 2019 10:37:53am
    Author:  Ian Caburian

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
class MouseLogger;

//==============================================================================

class Console : public DocumentWindow
{
public:
    //==============================================================================
    
    explicit Console();
    ~Console() = default;
    void closeButtonPressed() override;

private:
    //==============================================================================
    
    struct Content : public Component
    {
        struct Log : public Logger
        {
            StringArray event_times, event_types;
            static constexpr int history_lines = 20;
            Content& content;
            
            explicit Log(Content& content);
            ~Log() = default;
            void logMessage (const String& message) override;
        };
        Log log{ *this };
        TextEditor mouse_txt, parent_txt;
        
        explicit Content();
        ~Content();
        void resized() override;
    };
};
