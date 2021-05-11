#include "BlockClock.h"

class MyJUCEApp : public juce::JUCEApplicationBase
{
public:
    MyJUCEApp()  {}
    ~MyJUCEApp() {}

    void initialise (const juce::String&) override {}
    void shutdown() override                       {}

    const juce::String getApplicationName() override           { return "BlockClock"; }
    const juce::String getApplicationVersion() override        { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override                 { return true; }
    void anotherInstanceStarted (const juce::String&) override {}

    void suspended() override           {}
    void resumed() override             {}
    void systemRequestedQuit() override {}
    void unhandledException(const std::exception*,
                            const juce::String&,
                            int lineNumber) override {}

private:
    // Our BLOCKS class.
    BlockClock clock;
};

START_JUCE_APPLICATION(MyJUCEApp)
