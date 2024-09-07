def func():
    return [1, 2, 3]

a = func()
l = a
a[0] += 1;
print(l)

a = func()
print(a)
print(l)
