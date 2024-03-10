#pragma once
#include "util.h"
#include <mfwpch.h>

namespace mfw {
    class Input {
        friend class Application;
    public:
        virtual ~Input() {}
        inline static bool KeyPress(i32 key) { return Instance->KeyPressImpl(key); }
        inline static bool MouseButtonDown(i32 button) { return Instance->MouseButtonDownImpl(button); }
        inline static const std::pair<i32, i32> GetMouse() {return Instance->GetMouseImpl(); }

    private:
        virtual bool KeyPressImpl(const i32& key) = 0;
        virtual bool MouseButtonDownImpl(const i32& button) = 0;
        virtual const std::pair<i32, i32> GetMouseImpl() = 0;

        static Input* Instance;

    };

    enum MouseButton : u8 {
        Right,
        Left,
        Middle,
        X1,
        X2,
    };

}
