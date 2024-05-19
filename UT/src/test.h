#pragma once
#include <chrono>
#include <cstdio>
#include <vector>
#include <string>

struct TestObject;
inline std::vector<TestObject*> tests;

struct TestObject {
    TestObject(std::string unit_name, std::string test_name): unit_name(unit_name), test_name(test_name) {}
    inline virtual void run(int& success) = 0;
    std::string unit_name;
    std::string test_name;

};

#define UNIT(unit_name, test)\
    class unit_name##_##test : TestObject {\
    public:\
        unit_name##_##test(std::string unit_name, std::string test_name): TestObject(unit_name, test_name) { tests.push_back(this); }\
        inline virtual void run(int& success) override;\
    };\
    [[maybe_unused]]\
    inline unit_name##_##test t(#unit_name, #test);\
    inline void unit_name##_##test::run([[maybe_unused]] int& success)

int main() {
    const int total_tests = tests.size();
    int total_tests_failed = 0;
    for (auto& e : tests) {
        int success = 0;
        auto t1 = std::chrono::high_resolution_clock::now();
        e->run(success);
        auto t2 = std::chrono::high_resolution_clock::now();
        total_tests_failed += 1 - success;
        const float duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        std::printf("[%s][%s][%s](%gms)\n", e->unit_name.c_str(), e->test_name.c_str(), success ? "OK" : "FAILED", duration);
    }
    std::printf("failed %d in total %d test(s) with the success rate of %d%%", total_tests_failed, total_tests, 100 - (int)(100.f * (float)total_tests_failed / total_tests));
}

