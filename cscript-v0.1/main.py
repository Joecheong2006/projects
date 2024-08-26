def func():
    ar = [1, 2, 3]
    return ar

a = func()
a[2] = 0;
a = func()
print(a)
