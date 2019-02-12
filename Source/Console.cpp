/*
  ==============================================================================

    Console.cpp
    Created: 12 Feb 2019 10:37:53am
    Author:  Ian Caburian

  ==============================================================================
*/

#include "Console.h"
Console::Console()
: DocumentWindow{
    "Live Shader Palette - Console",
    Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
    DocumentWindow::allButtons
  }
{
    setContentOwned(new Content{}, true);
    setUsingNativeTitleBar (true);
    setResizable (true, true);
    setBounds(0, 0, getWidth(), getHeight());
    setVisible (true);
}
void Console::closeButtonPressed()
{
    
}

//==============================================================================

Console::Content::Content()
{
    for (auto* c : std::initializer_list<TextEditor*>{
        &mouse_txt, &parent_txt
    }) {
        c->setMultiLine(true);
        c->setReadOnly(true);
        c->setCaretVisible(false);
        c->setFont({ Font::getDefaultMonospacedFontName(), 14.0f, Font::plain });
        addAndMakeVisible(c);
    }
    setSize(800, 300);
    
    Logger::setCurrentLogger(&log);
}
Console::Content::~Content()
{
    Logger::setCurrentLogger(nullptr);
}
void Console::Content::resized()
{
    auto bounds = getLocalBounds();
    parent_txt.setBounds(bounds.removeFromTop(50));
    mouse_txt.setBounds(bounds);
}

//==============================================================================

Console::Content::Log::Log(Content& content)
: content{ content }
{
}
void Console::Content::Log::logMessage(const String& message)
{
    const auto tag = message.upToFirstOccurrenceOf(" ", false, true);
    auto body = message.fromFirstOccurrenceOf(" ", false, true);
    if (tag == "m") {
        
        auto history = [&](const String& start, StringArray& buffer) {
            const auto token = String{ body.fromFirstOccurrenceOf(start, true, false)
                                           .upToFirstOccurrenceOf("|", false, true) };
            if (buffer.size() > history_lines) {
                buffer.remove(0);
            }
            buffer.add(token);
        };
        history("Type: ", event_types);
        history("Event Time: ", event_times);
        body << "\n\nHistory:";
        for (int i = history_lines; --i >= 0;) {
            body << "\n" << event_types[i].paddedRight(' ', 16) << " | " << event_times[i];
        }
        content.mouse_txt.setText(body);
    }
    else if (tag == "p") {
        content.parent_txt.setText(body);
    }
}
