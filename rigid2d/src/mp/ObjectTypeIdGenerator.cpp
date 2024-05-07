#include "ObjectTypeIdGenerator.h"

i32 ObjectTypeIdGenerator::current;

i32 ObjectTypeIdGenerator::GenerateId() {
    return current++;
}

i32 ObjectTypeIdGenerator::GetCurrent() {
    return current;
}

