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

#ifndef _CLI_SUBMENU_H_
#define _CLI_SUBMENU_H_

#include <span>
#include <vector>
#include "IODevice/IODevice.h"
#include "menuItem.h"

namespace microhal {

class MainMenuBase;
/**
 * @brief Provides sub-folder functionalities.
 */
class SubMenuBase : public MenuItem {
    friend MainMenuBase;

 protected:
    /**
     * @brief Constructs sub folder of given name and default help description.
     * @param name - sub folder name.
     * @param help - default help description.
     */
    SubMenuBase(std::string_view name, std::span<MenuItem*> items) : MenuItem(name), items(items) {}

    /**
     * @brief	Function for recognition whether it has children list or not.
     * @return True
     */

    bool hasChildrens(void) final { return true; }

 protected:
    /**
     * @brief Vector of object in sub folder.
     */
    std::span<MenuItem*> items;
};

template <size_t size>
class SubMenu : public SubMenuBase {
 public:
    template <typename... Args>
    SubMenu(std::string_view name, Args&... args) : SubMenuBase(name, itemsContainer), itemsContainer({&args...}) {}

 private:
    std::array<MenuItem*, size> itemsContainer{};
};

template <>
class SubMenu<std::dynamic_extent> : public SubMenuBase {
 public:
    template <typename... Args>
    SubMenu(std::string_view name, Args&... args) : SubMenuBase(name, itemsContainer), itemsContainer({&args...}) {}

    SubMenu(std::string_view name) : SubMenuBase(name, itemsContainer), itemsContainer({}) {}
    /**
     * @brief Adds an MenuItem into sub folder.
     * @param item - MenuItem reference which should be added into sub folder.
     */
    inline void addItem(MenuItem& item) {
        itemsContainer.push_back(&item);
        SubMenuBase::items = items;
    }
    /**
     * @brief Adds an SubMenu into sub folder.
     * @param item - SubMenu reference which should be added into sub folder.
     */
    inline void addItem(SubMenuBase& item) {
        itemsContainer.push_back(&item);
        SubMenuBase::items = items;
    }

 private:
    std::vector<MenuItem*> itemsContainer{};
};

}  // namespace microhal

#endif /* _CLI_SUBMENU_H_ */

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
