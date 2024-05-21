#include "../src/test.h"

extern "C" {
    void func() {}
};

UNIT(test, test) {
    success = 1;
    func();
}
