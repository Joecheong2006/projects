#pragma once

#include "Clock.h"

namespace mfw {
    class WindowsTime : public Time {
    public:
        WindowsTime();

    private:
        virtual f64 GetCurrentImpl() override;
        virtual void SleepImpl(f32 millisecond) override;

        f32 pcfreq;
        i64 start;

    };
}

