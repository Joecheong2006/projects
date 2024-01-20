#pragma once
#include "util.h"

namespace mfw {
    class Input {
    public:
        inline static bool KeyPress(i32 key) { return instance->KeyPressImpl(key); }
        inline static bool MouseButtonDown(i32 button) { return instance->MouseButtonDownImpl(button); }

    private:
        virtual bool KeyPressImpl(const i32& key) = 0;
        virtual bool MouseButtonDownImpl(const i32& button) = 0;

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
