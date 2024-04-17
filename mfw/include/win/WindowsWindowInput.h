#pragma once
#include "Input.h"

namespace mfw {
    class WindowsWindowInput : public Input {
    public:
        WindowsWindowInput();

    private:
        virtual bool KeyPressImpl(const i32& key) override;
        virtual bool MouseButtonDownImpl(const i32& button) override;
        virtual const std::pair<i32, i32> GetMouseImpl() override;
        virtual i32 GetKeyCodeImpl(i32 key) override;

        //i32 mfKeyMaps[520];
        std::unordered_map<i32, i32> keymap;

    };
}
