#pragma once
#include "mfw/util.h"

class ObjectTypeIdGenerator {
private:
    static i32 current;

public:
    static i32 GenerateId();
    static i32 GetCurrent();

};

