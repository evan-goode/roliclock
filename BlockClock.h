#pragma once

#include <string>
#include <ctime>

#include <BlocksHeader.h>

#include "font.h"

#define FPS 25
#define DISPLAY_SIZE 15
#define DISPLAY_ROTATE 2

// Monitors a PhysicalTopologySource for changes to the connected BLOCKS and
// prints some information about the BLOCKS that are available.
class BlockClock : private juce::TopologySource::Listener,
                   private juce::TouchSurface::Listener,
                   private juce::HighResolutionTimer {
public:
    // Register as a listener to the PhysicalTopologySource, so that we receive
    // callbacks in topologyChanged().
    BlockClock();

private:
    juce::Block::Ptr activeBlock;

    // Called by the PhysicalTopologySource when the BLOCKS topology changes.
    void topologyChanged() override;

    void touchChanged(juce::TouchSurface& sourceTouchSurface, const juce::TouchSurface::Touch& touchEvent) override;

    void hiResTimerCallback() override;

    void setBitmapLEDProgram();

    void redraw();

    void setLED(int x, int y, juce::LEDColour color);

    void drawRect(int posX, int posY, int width, int height, juce::LEDColour color);

    void drawNumber(int number, int x, int y, juce::LEDColour color);

    void drawTime();


    // The PhysicalTopologySource member variable which reports BLOCKS changes.
    juce::PhysicalTopologySource pts;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlockClock)
};
