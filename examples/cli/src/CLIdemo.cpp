/**
 * @license    BSD 3-Clause
 * @copyright  microHAL
 * @version    $Id$
 * @brief	   Example of a project that incorporates CLI module.
 *
 * @authors    Kubaszek Mateusz <mathir.km.microhal@gmail.com>
 * created on: <23-01-2016>
 * last modification: <24-01-2016>
 *
 * @copyright Copyright (c) 2015, microHAL
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *        2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 *           documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
 *        software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup microHAL
 * @{
 */

/**
 * @defgroup components
 * @{
 */

/**
 * @defgroup cli
 * @{
 */

/**
 * @defgroup example
 * @{
 */

#include "CLI.h"
#include "cliParser.h"
#include "microhal.h"
#include "microhal_bsp.h"
#include "parsers/argumentParser.h"
#include "parsers/numericParser.h"
#include "parsers/stringParser.h"
#include "subMenu.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>

using namespace microhal;

// The following classes are menu item objects containing executing static functions.

class MemorySave : public MenuItem {
 public:
    MemorySave(void) : MenuItem("save") {}
    static void memorySave(IODevice& port) { port.write("Memory saved!!!"); }

 protected:
    int execute([[maybe_unused]] std::list<char*>& words, IODevice& port) {
        memorySave(port);
        return 0;
    }
};

class MemoryRestore : public MenuItem {
 public:
    MemoryRestore(void) : MenuItem("restore") {}
    static void memoryRestore(IODevice& port) { port.write("Memory restored!!!"); }

 protected:
    int execute([[maybe_unused]] std::span<std::string_view> words, IODevice& port) final {
        memoryRestore(port);
        return 0;
    }
};

class Car : public MenuItem {
 public:
    static char color[30];
    static float maxSpeed;
    static int gearsCnt;

    Car(std::string_view name) : MenuItem(name) {}

 protected:
    void setColor(std::string_view color) {
        std::copy_n(color.begin(), color.size(), this->color);
        this->color[color.size()] = 0;
    }
};

char Car::color[30] = "undefined";
float Car::maxSpeed = -1.0;
int Car::gearsCnt = -1;

class CarSet : public Car {
 public:
    CarSet(void) : Car("set") {}

 protected:
    int execute(std::span<std::string_view> words, IODevice& port) final {
        std::string str;
        for (auto& word : words) {
            str += word;
            str += " ";
        }
        cli::StringParser color(-1, "color", "color", "color name as string", 1, 40);
        cli::NumericParser<float> speed(-1, "speed", "speed", "max speed of car", 50.0f, 400.0f);
        cli::NumericParser<int> gears(-1, "gears", "gears", "gears count", 3, 20);
        cli::ArgumentParser parser("set", "Set car parameters");
        parser.addArgument(color);
        parser.addArgument(speed);
        parser.addArgument(gears);
        if (auto status = parser.parse(str, port); status == cli::Status::Success) {
            port.write("\tSet color to ");
            port.write(color.value());
            port.write(".\n");
            setColor(color.value());

            char txt[60];
            snprintf(txt, 60, "\tSet maxspeed to %f.\n", speed.value());
            port.write(txt);
            maxSpeed = speed.value();

            snprintf(txt, 60, "\tSet gears count to %d.\n", gears.value());
            port.write(txt);
            gearsCnt = gears.value();
        } else if (status != cli::Status::HelpRequested) {
            port.write("Incorrect argument.");
        }

        return 0;
    }
};

class CarPrint : public Car {
 public:
    CarPrint(void) : Car("get") {}

 protected:
    int execute([[maybe_unused]] std::span<std::string_view> words, IODevice& port) final {
        char txt[30];
        port.write("Your car is ");
        port.write(color);
        port.write(", its max speed is ");
        snprintf(txt, 30, "%f, and has %d gears.\n", Car::maxSpeed, Car::gearsCnt);
        port.write(txt);
        return 0;
    }
};

class Clock : public MenuItem {
 public:
    Clock(std::string_view name) : MenuItem(name) {}

    static int hours, minutes, seconds;
    static bool alarm;
};

int Clock::hours = 0;
int Clock::minutes = 0;
int Clock::seconds = 0;
bool Clock::alarm = false;

class AlarmOff : public Clock {
 public:
    AlarmOff(void) : Clock("off") {}
    static void alarmOff(IODevice& port) {
        alarm = false;
        port.write("AlarmOff");
    }

 protected:
    int execute([[maybe_unused]] std::span<std::string_view> words, IODevice& port) final {
        alarmOff(port);
        return 0;
    }
};

class AlarmOn : public Clock {
 public:
    AlarmOn(void) : Clock("on") {}
    static void alarmOn(IODevice& port) {
        alarm = true;
        port.write("AlarmOn");
    }

 protected:
    int execute([[maybe_unused]] std::span<std::string_view> words, IODevice& port) final {
        alarmOn(port);
        return 0;
    }
};

class ClockStatus : public Clock {
 public:
    ClockStatus(void) : Clock("status") {}
    static void clockStatus(IODevice& port) {
        char str[25];
        snprintf(str, 25, "%02d:%02d:%02d\n", hours, minutes, seconds);
        port.write(str);
        if (alarm)
            port.write("Alarm is on.");
        else
            port.write("Alarm is off.");
    }

 protected:
    int execute([[maybe_unused]] std::span<std::string_view> words, IODevice& port) final {
        clockStatus(port);
        return 0;
    }
};

class ClockSet : public Clock {
 public:
    ClockSet(void) : Clock("set") {}

 protected:
    int execute(std::span<std::string_view> words, IODevice& port) final {
        std::string str;
        for (auto& word : words) {
            str += word;
            str += " ";
        }
        cli::NumericParser<int> sec('s', {}, "seconds", "Seconds from 0 to 59.", 0, 59);
        cli::NumericParser<int> min('m', {}, "minutes", "Minutes from 0 to 59.", 0, 59);
        cli::NumericParser<int> hrs(-1, "hr", "hours", "Hours from 0 to 23.", 0, 23);
        cli::ArgumentParser parser("set", "Set current time.");
        parser.addArgument(sec);
        parser.addArgument(min);
        parser.addArgument(hrs);
        if (parser.parse(str, port) == cli::Status::Success) {
            seconds = sec.value();
            minutes = min.value();
            hours = hrs.value();

            char txt[60];
            snprintf(txt, 60, "\tCurrent time is %02d:%02d:%02d\n", hours, minutes, seconds);
            port.write(txt);
        } else {
            port.write("Incorrect parameter.");
        }
        return 0;
    }
};

/**
 * @brief   Main function of example CLI project. Consist of menu initialization and
 *          CLI chars reading loop.
 * @return  Maybe you are a golfer?
 */
int main(void) {
    debugPort.open(IODevice::ReadWrite);

    MainMenu _root(debugPort);

    SubMenu _clock("clock");
    _root.addItem(_clock);

    SubMenu _car("car");
    _root.addItem(_car);
    CarPrint carPrint;
    CarSet carSet;
    _car.addItem(carPrint);
    _car.addItem(carSet);

    ClockStatus cstat;
    _clock.addItem(cstat);
    ClockSet cset;
    _clock.addItem(cset);
    SubMenu _alarm("alarm");
    _clock.addItem(_alarm);

    SubMenu _emptySet("empty");
    _root.addItem(_emptySet);
    SubMenu _recursiveEmptySet("empty");
    _emptySet.addItem(_recursiveEmptySet);

    AlarmOn aon;
    _alarm.addItem(aon);
    AlarmOff aoff;
    _alarm.addItem(aoff);

    SubMenu _memory("memory");
    _root.addItem(_memory);

    MemorySave ms;
    MemoryRestore mr;
    _memory.addItem(ms);
    _memory.addItem(mr);

    CLI cli(debugPort, _root, "\n\r---------------------------- CLI DEMO -----------------------------\n\r");

    std::chrono::milliseconds dura(100);
    while (1) {
        cli.readInput();
        std::this_thread::sleep_for(dura);
    }
    return 0;
}

/**
 * example
 * @}
 */

/**
 * cli
 * @}
 */

/**
 * components
 * @}
 */

/**
 * microHAL
 * @}
 */
