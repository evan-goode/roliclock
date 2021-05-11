#include "BlockClock.h"

using namespace juce;

BlockClock::BlockClock() {
    // Register to receive topologyChanged() callbacks from pts.
    pts.addListener(this);
}

void BlockClock::topologyChanged() {
    // We have a new topology, so find out what it isand store it in a local
    // variable.
    auto currentTopology = pts.getCurrentTopology();
    Logger::writeToLog ("\nNew BLOCKS topology.");

    // The blocks member of a BlockTopology contains an array of blocks. Here we
    // loop over them and print some information.
    Logger::writeToLog ("Detected " + String (currentTopology.blocks.size()) + " blocks:");

    for (auto& block : currentTopology.blocks) {
        Logger::writeToLog ("");
        Logger::writeToLog ("    Description:   " + block->getDeviceDescription());
        Logger::writeToLog ("    Battery level: " + String (block->getBatteryLevel()));
        Logger::writeToLog ("    UID:           " + String (block->uid));
        Logger::writeToLog ("    Serial number: " + block->serialNumber);
        if (block->getType() == Block::Type::lightPadBlock) {
            activeBlock = block;
            activeBlock->setProgram(new BitmapLEDProgram(*activeBlock));
            activeBlock->getTouchSurface()->addListener(this);
            if (!isTimerRunning()) {
                startTimer(1000 / FPS);
            }
            return;
        }
    }
    stopTimer();
}

void BlockClock::drawRect(int posX, int posY, int width, int height, juce::LEDColour color) {
    for (int y = 0; y < height; y += 1) {
        for (int x = 0; x < width; x += 1) {
            setLED(posX + x, posY + y, color);
        }
    }
}

void BlockClock::drawNumber(int number, int posX, int posY, juce::LEDColour color) {
    for (int y = 0; y < CHAR_HEIGHT; y += 1) {
        for (int x = 0; x < CHAR_WIDTH; x += 1) {
            if (numbers[number][y][x]) {
                setLED(posX + x, posY + y, color);
            }
        }
    }
}

void BlockClock::drawTime() {
    auto color = juce::LEDColour(0xffffffff);

    std::time_t time = std::time(0);
    std::tm* localtime = std::localtime(&time);
    
    int hourTens = (localtime->tm_hour / 10) % 10;
    int hourOnes = localtime->tm_hour % 10;
    int minTens = (localtime->tm_sec / 10) % 10;
    int minOnes = localtime->tm_sec % 10;

    drawNumber(hourTens, 0, 5, color);
    drawNumber(hourOnes, 4, 5, color);

    setLED(7, 6, LEDColour(0xffff0000));
    setLED(7, 8, LEDColour(0xffff0000));

    drawNumber(minTens, 8, 5, color);
    drawNumber(minOnes, 12, 5, color);
}

void BlockClock::touchChanged(juce::TouchSurface& sourceTouchSurface, const juce::TouchSurface::Touch& touchEvent) {
    /* Logger::writeToLog("touch changed"); */
}

void BlockClock::setLED(int posX, int posY, juce::LEDColour color) {
    if (auto program = dynamic_cast<juce::BitmapLEDProgram*> (activeBlock->getProgram())) {
        int x, y;
        if (DISPLAY_ROTATE == 0) {
            x = posX;
            y = posY;
        } else if (DISPLAY_ROTATE == 1) {
            x = posY;
            y = DISPLAY_SIZE - 1 - posX;
        } else if (DISPLAY_ROTATE == 2) {
            x = DISPLAY_SIZE - 1 - posX;
            y = DISPLAY_SIZE - 1 - posY;
        } else if (DISPLAY_ROTATE == 3) {
            x = DISPLAY_SIZE - 1 - posY;
            y = posX;
        }
        program->setLED(x, y, color);
    }
}

void BlockClock::redraw() {
    drawRect(0, 0, DISPLAY_SIZE, DISPLAY_SIZE, juce::LEDColour(0xff000000));
    drawTime();
}

void BlockClock::hiResTimerCallback() {
    redraw();
}
