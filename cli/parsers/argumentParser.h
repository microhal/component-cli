/**
 * @license    BSD 3-Clause
 * @copyright  Pawel Okas
 * @version    $Id$
 * @brief
 *
 * @authors    Pawel Okas
 *
 * @copyright Copyright (c) 2021, Pawel Okas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
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

#ifndef SRC_CLI_PARSERS_ARGUMENTPARSER_H_
#define SRC_CLI_PARSERS_ARGUMENTPARSER_H_

#include <string_view>
#include <vector>
#include "argument.h"
#include "status.h"

namespace microhal {
namespace cli {

class ArgumentParser {
 public:
    using string_view = std::string_view;

    ArgumentParser(string_view name, string_view description) : name(name), description(description) {}

    void addArgument(Argument &arg) { arguments.push_back(&arg); }

    [[nodiscard]] Status parse(std::string_view argumentsString, IODevice &ioDevice);

    void showUsage(IODevice &ioDevice);

 private:
    bool isHelpArgument(std::string_view);
    std::string_view getParameters(std::string_view arguments, int8_t argumentsCount);

    std::vector<Argument *> arguments{};
    std::string_view name;
    std::string_view description;

    [[nodiscard]] constexpr static string_view removeSpaces(string_view str) {
        // remove leading spaces
        if (auto pos = str.find_first_not_of(' '); pos != str.npos) str.remove_prefix(pos);
        // remove trailing spaces
        if (auto pos = str.find_last_not_of(' '); pos != str.npos) str.remove_suffix(str.size() - pos - 1);
        return str;
    }
};

}  // namespace cli
}  // namespace microhal

#endif /* SRC_CLI_PARSERS_ARGUMENTPARSER_H_ */
