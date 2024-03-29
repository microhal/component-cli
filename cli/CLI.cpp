/**
 * @license    BSD 3-Clause
 * @copyright  microHAL
 * @version    $Id$
 * @brief    Example of a project that incorporates CLI module.
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

#include "CLI.h"

using namespace std::literals;

namespace microhal {

void CLI::addSign(char sign) {
    /* Arrow sign is 0xe0 followed by: 0x48 (up), 0x50 (down), 0x4b (left), 0x4d (right) */
    static uint8_t arrowSign = 0;
    static uint8_t previous_CR = 0;
    constexpr uint8_t maxLen = LINELENGTH - 2; /* Space and NULL termination */

    if (previous_CR) {
        if (sign == '\n') return;
        previous_CR = 0;
    }

    /* In case previous char was arrow sign */
    if (arrowSign == 2) {
        arrowSign = 0;
        switch (sign) {
            /* UP */
            case 'A':
                showPreviousCommand();
                return;
                /* DOWN */
            case 'B':
                showNextCommand();
                return;
                /* LEFT */
            case 'D':
                /* RIGTH */
            case 'C':
                return;
        }
    }

    switch (sign) {
        case '\b':
        case 127:
            if (previousBuffer != activeBuffer) duplicateCommand();
            if (charDelete(1)) cleanLine(1);
            return;
        case '\t':
            if (previousBuffer != activeBuffer) duplicateCommand();
            showCommands();
            return;
            break;

        case '\r':
            previous_CR = 1;
        case '\n':
            if (previousBuffer != activeBuffer) duplicateCommand();
            processBuffer();
            return;
            break;
    }

    switch (sign) {
        case 27:
            if (arrowSign == 0) arrowSign = 1;
            return;
            break;
        case 91:
            if (arrowSign == 1) arrowSign = 2;
            return;
            break;
        default:
            arrowSign = 0;
    }

    if (length < maxLen) {
        if (previousBuffer != activeBuffer) duplicateCommand();
        // charAppend(sign);
        if (sign == charAppend(sign)) port.write(&sign, 1);
    }
}

void CLI::showPreviousCommand() {
    uint8_t tmpPrev = previousBuffer;
    if (tmpPrev == 0) {
        tmpPrev = BUFFERLENGTH - 1;
    } else {
        tmpPrev--;
    }
    if (tmpPrev != activeBuffer) {
        /* If previous command exists */
        if (dataBuffer[tmpPrev][0] != '\0') {
            cleanLine(length);  // Cleaning current line
            charAppend(0);
            /* Setting line from history, and printing its content */
            previousBuffer = tmpPrev;
            length = strlen(dataBuffer[previousBuffer]);
            port.write(dataBuffer[previousBuffer]);
        }
    }
}

void CLI::showNextCommand() {
    uint8_t tmpPrev = previousBuffer;
    if (tmpPrev != activeBuffer) {
        tmpPrev = (tmpPrev + 1) % BUFFERLENGTH;
        cleanLine(length);
        charAppend(0);
        previousBuffer = tmpPrev;
        length = strlen(dataBuffer[previousBuffer]);
        port.write(dataBuffer[previousBuffer]);
    }
}

void CLI::duplicateCommand() {
    memcpy(dataBuffer[activeBuffer], dataBuffer[previousBuffer], length);
    previousBuffer = activeBuffer;
}

void CLI::processBuffer() {
    if (length != 0) {
        auto buf = std::string_view(dataBuffer[activeBuffer], length);
        auto command = buf;
        std::string_view parameters{};
        if (auto pos = buf.find(' '); pos != buf.npos) {
            command = buf.substr(0, pos);
            parameters = buf.substr(pos + 1);
        }
        menu.processCommand(command, parameters);
        activeBuffer = (activeBuffer + 1) % BUFFERLENGTH;
        length = 0;
        dataBuffer[activeBuffer][0] = '\0';
        previousBuffer = activeBuffer;
    }
    drawPrompt();
}

void CLI::init() {
    // clear buffer
    for (uint32_t index = 0; index < BUFFERLENGTH; index++) {
        dataBuffer[index][0] = '\0';
    }

    // draw prompt on screen
    drawPrompt();
}

void CLI::showCommands() {
    const auto buf = std::string_view(dataBuffer[activeBuffer], length);
    auto command = buf;
    if (auto pos = buf.find(' '); pos != buf.npos) {
        command = buf.substr(0, pos);
    }
    /* Last word is containing the sentence that should be complemented */
    std::string_view ret = menu.showCommands(command);
    if (ret.size() > 0) {
        /* ret number of letters were appended */
        const uint16_t spaceInBuffer = LINELENGTH - length;
        if (ret.size() < spaceInBuffer) {
            ret.copy(&dataBuffer[activeBuffer][buf.size()], ret.size());
            /* Clearing current line */
            cleanLine(length);
            length += ret.size();
            dataBuffer[activeBuffer][length] = 0;
        } else {
            port.write("\n\rLine buffer overflow.\n\r"sv);
        }
    }
    if (ret.empty()) {
        /* Some text has been displayed, puts new prompt and rewrite command */
        drawPrompt();
    }
    port.write({dataBuffer[activeBuffer], length});
}

}  // namespace microhal

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
