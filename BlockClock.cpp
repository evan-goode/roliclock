#include "BlockClock.h"

using namespace juce;

BlockClock::BlockClock() {
    lightsdPipe = new juce::NamedPipe();
    lightsdPipe->openExisting(LIGHTSD_PIPE_PATH);
    /* juce::LEDColour *fb[DISPLAY_SIZE][DISPLAY_SIZE]; */
    for (int y = 0; y < DISPLAY_SIZE; y += 1) {
        fb.push_back(std::vector<juce::LEDColour>());
        for (int x = 0; x < DISPLAY_SIZE; x += 1) {
            fb[y].push_back(juce::LEDColour(0xff000000));
        }
    }
    pts.addListener(this);
}

void BlockClock::topologyChanged() {
    // We have a new topology, so find out what it isand store it in a local
    // variable.
    auto currentTopology = pts.getCurrentTopology();
    Logger::writeToLog ("\nNew BLOCKS topology.");

    // The blocks member of a BlockTopology contains an array of blocks. Here we
    // loop over them and print some information.
    /* Logger::writeToLog ("Detected " + String (currentTopology.blocks.size()) + " blocks:"); */

    for (auto& block : currentTopology.blocks) {
        /* Logger::writeToLog (""); */
        /* Logger::writeToLog ("    Description:   " + block->getDeviceDescription()); */
        /* Logger::writeToLog ("    Battery level: " + String (block->getBatteryLevel())); */
        /* Logger::writeToLog ("    UID:           " + String (block->uid)); */
        /* Logger::writeToLog ("    Serial numbr: " + block->serialNumber); */
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
            drawPixel(posX + x, posY + y, color);
        }
    }
}

void BlockClock::drawNumber(int number, int posX, int posY, juce::LEDColour color) {
    for (int y = 0; y < CHAR_HEIGHT; y += 1) {
        for (int x = 0; x < CHAR_WIDTH; x += 1) {
            if (numbers[number][y][x]) {
                drawPixel(posX + x, posY + y, color);
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
    int minTens = (localtime->tm_min / 10) % 10;
    int minOnes = localtime->tm_min % 10;

    drawNumber(hourTens, 0, 0, color);
    drawNumber(hourOnes, 4, 0, color);

    drawPixel(7, 1, LEDColour(0xffff0000));
    drawPixel(7, 3, LEDColour(0xffff0000));

    drawNumber(minTens, 8, 0, color);
    drawNumber(minOnes, 12, 0, color);
}

void BlockClock::touchChanged(juce::TouchSurface& touchSurface, const juce::TouchSurface::Touch& touch) {
    float rawX = touch.x / activeBlock->getWidth();
    float rawY = touch.y / activeBlock->getHeight();

    float x, y;
    if (DISPLAY_ROTATE == 0) {
        x = rawX;
        y = 1 - rawY;
    } else if (DISPLAY_ROTATE == 1) {
        x = rawY;
        y = rawX;
    } else if (DISPLAY_ROTATE == 2) {
        x = 1 - rawX;
        y = rawY;
    } else if (DISPLAY_ROTATE == 3) {
        x = 1 - rawY;
        y = 1 - rawX;
    }

    float saturation = x;
    float brightness = y;

    std::string payload = "{"
    "	\"jsonrpc\": \"2.0\","
    "	\"method\": \"set_light_from_hsbk\","
    "	\"params\": [\"*\", 30, " + std::to_string(saturation) + ", "+ std::to_string(brightness) + ", 3500, 50]"
    "}\n";
    /* std::string payload = "{" */
    /* "	\"jsonrpc\": \"2.0\"," */
    /* "	\"method\": \"set_light_from_hsbk\"," */
    /* "	\"params\": [\"*\", 0, 0.5, " + std::to_string(brightness) + ", 0.5, 3500, 50]" */
    /* "}\n"; */

    /* if (!result.wasOk()) { */
    /*     Logger::writeToLog("was not ok" + result.getErrorMessage()); */
    /* } */

    char const *c = payload.c_str();
    if (lightsdPipe->isOpen()) {
        Logger::writeToLog("yaa");
    }
    lightsdPipe->write((const void*) c, (int) payload.length(), 100);
    Logger::writeToLog(payload);

    /* Logger::writeToLog("touch changed" + String(touch.x / activeBlock->getWidth()) + ", " + String(touch.y / activeBlock->getHeight())); */
}

void BlockClock::drawPixel(int posX, int posY, juce::LEDColour color) {
    fb[posY][posX] = color;
}

void BlockClock::sendFb() {
    if (auto program = dynamic_cast<juce::BitmapLEDProgram*> (activeBlock->getProgram())) {
        for (int posY = 0; posY < fb.size(); posY += 1) {
            for (int posX = 0; posX < fb[posY].size(); posX += 1) {
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
                program->setLED(x, y, fb[posY][posX]);
            }
        }
    }
}

void BlockClock::redraw() {
    drawRect(0, 0, DISPLAY_SIZE, DISPLAY_SIZE, juce::LEDColour(0xff000000));
    drawTime();
    sendFb();
}

void BlockClock::hiResTimerCallback() {
    redraw();
}
