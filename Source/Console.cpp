/*
  ==============================================================================

    Console.cpp
    Created: 12 Feb 2019 10:37:53am
    Author:  Ian Caburian

  ==============================================================================
*/

#include "Console.h"
#include "ToolBar.h"
#include "MainComponent.h"
//==============================================================================

Console::Console(MainComponent& main_window)
: DocumentWindow{
    "Live Shader Palette - Console",
    Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
    DocumentWindow::closeButton
  }
, main_window{ main_window }
{
    setContentOwned(new Content{ *this }, true);
    setUsingNativeTitleBar (true);
    setResizable (true, true);
    setVisible (true);
}
void Console::closeButtonPressed()
{
    main_window.open_console(false);
}

// Console::Content ============================================================

Console::Content::Content(Console& console)
: console{ console }
{
    for (auto* t : std::initializer_list<TextEditor*>{
        &mouse_txt, &parent_txt, &compiler_txt
    }) {
        t->setMultiLine(true, false);
        t->setReadOnly(true);
        t->setCaretVisible(false);
        addAndMakeVisible(t);
    }
    
    compile_rate_sld.getValueObject().referTo(console.main_window.get_compile_rate_val());
    compile_rate_sld.setSliderStyle(Slider::SliderStyle::LinearBar);
    compile_rate_sld.setTextBoxIsEditable(true);
    compile_rate_sld.setRange(500., 10000.);
    compile_rate_sld.onValueChange = [this] {
        if (this->console.main_window.is_live_compiling()) {
            this->console.main_window.stopTimer();
            this->console.main_window.startTimer(compile_rate_sld.getValue());
        }
    };
    compile_rate_lbl.setText("Live Compile Rate (ms)", dontSendNotification);
    addAndMakeVisible(compile_rate_sld);
    addAndMakeVisible(compile_rate_lbl);

    period_sld.getValueObject().referTo(console.main_window.get_period_val());
    period_sld.setSliderStyle(Slider::SliderStyle::LinearBar);
    period_sld.setTextBoxIsEditable(true);
    period_sld.setRange(10., 10000.);
    period_lbl.setText("Period (ms)", dontSendNotification);
    addAndMakeVisible(period_sld);
    addAndMakeVisible(period_lbl);

    const auto main_display = Desktop::getInstance().getDisplays().getMainDisplay();
    setSize(main_display.userArea.proportionOfWidth(0.4f), main_display.userArea.proportionOfHeight(0.4f));
    Logger::setCurrentLogger(&log);
}
Console::Content::~Content()
{
    Logger::setCurrentLogger(nullptr);
}
void Console::Content::resized()
{
    const auto row_height = Desktop::getInstance().getDisplays().getMainDisplay()
                                                  .userArea.proportionOfHeight(row_scalar);
    const auto font = ToolBar::mono_font(row_height);
    compile_rate_lbl.setFont(font);
    period_lbl.setFont(font);
    for (auto* t : std::initializer_list<TextEditor*>{
        &mouse_txt, &parent_txt, &compiler_txt
    }) {
        t->applyFontToAllText(font);
    }
    
    auto bounds = getLocalBounds();
    auto top_bounds = bounds.removeFromTop(row_height * 5);
    auto slider_bounds = top_bounds.removeFromRight(proportionOfWidth(0.4f));
    
    for (auto* c : std::initializer_list<Component*> {
        &compile_rate_lbl, &compile_rate_sld, &period_lbl, &period_sld
    }) {
        c->setBounds(slider_bounds.removeFromTop(row_height));
    }
    parent_txt.setBounds(top_bounds);
    mouse_txt.setBounds(bounds.removeFromTop(row_height * 10));
    compiler_txt.setBounds(bounds);
}

// Console::Content::Log =======================================================

Console::Content::Log::Log(Content& content)
: content{ content }
{
}
void Console::Content::Log::logMessage(const String& message)
{
    const auto tag = message.upToFirstOccurrenceOf(" ", false, true);
    auto body = message.fromFirstOccurrenceOf(" ", false, true);
    if (tag == "m") {
        
        const auto event_token = body.fromFirstOccurrenceOf("eventTime", true, false);
        if (events.size() > history_lines) {
            events.remove(0);
        }
        events.add(event_token);
        for (int i = std::min(history_lines, events.size()); --i >= 0;) {
            body << "\n        " << events[i];
        }
        content.mouse_txt.setText(body);
    }
    else {
        (tag == "p" ? content.parent_txt : content.compiler_txt).setText(body);
    }
}
