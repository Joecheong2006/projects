#include <mfwpch.h>
#pragma once

struct Test {
    std::string name;
    Test() = default;
    virtual ~Test() = default;
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void UpdateImgui() = 0;

};
