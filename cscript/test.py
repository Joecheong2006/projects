def funcA(i):
    if (i == 1000):
        return i
    return func(i + 1)

def func(i):
    if (i == 1000):
        return i
    return funcA(i + 1)

print(func(0))
