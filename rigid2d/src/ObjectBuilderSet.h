#pragma once

template <typename T, typename B>
class ObjectBuilderSet {
public:
    T* operator->() { return object; }
    operator B*() { return object; }

protected:
    T* object;

};
