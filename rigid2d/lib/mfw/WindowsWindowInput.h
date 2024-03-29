#pragma once
#include "Input.h"

namespace mfw {
    class WindowsWindowInput : public Input {
    private:
        virtual bool KeyPressImpl(const i32& key) override;
        virtual bool MouseButtonDownImpl(const i32& button) override;
        virtual const std::pair<i32, i32> GetMouseImpl() override;

    };
}
