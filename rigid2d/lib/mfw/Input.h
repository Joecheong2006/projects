#include "mfwpch.h"
#pragma once
#include "KeyCode.h"

#include "util.h"

namespace mfw {
    class Input {
        friend class Application;
    public:
        virtual ~Input() = default;
        inline static bool KeyPress(i32 key) { return Instance->KeyPressImpl(key); }
        inline static bool MouseButtonDown(i32 button) { return Instance->MouseButtonDownImpl(button); }
        inline static const std::pair<i32, i32> GetMouse() {return Instance->GetMouseImpl(); }
        inline static i32 GetKeyCode(i32 key) { return Instance->GetKeyCodeImpl(key); }

    private:
        virtual bool KeyPressImpl(const i32& key) = 0;
        virtual bool MouseButtonDownImpl(const i32& button) = 0;
        virtual const std::pair<i32, i32> GetMouseImpl() = 0;
        virtual i32 GetKeyCodeImpl(i32 key) = 0;

        static Input* Instance;

    };

}
