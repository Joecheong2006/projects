#pragma once
#include "util.h"
#include <mfwpch.h>

namespace mfw {
    class Input {
    public:
        inline static bool KeyPress(i32 key) { return instance->KeyPressImpl(key); }
        inline static bool MouseButtonDown(i32 button) { return instance->MouseButtonDownImpl(button); }
        inline static const std::pair<i32, i32> GetMouse() {return instance->GetMouseImpl(); }

    private:
        virtual bool KeyPressImpl(const i32& key) = 0;
        virtual bool MouseButtonDownImpl(const i32& button) = 0;
        virtual const std::pair<i32, i32> GetMouseImpl() = 0;

        static Input* instance;

    };

    enum MouseButton : u8 {
        Right,
        Left,
        Middle,
        X1,
        X2,
    };

}
