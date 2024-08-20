class Vec:
    x = 0
    y = 0


def get_vec():
    print("HI")
    return get_vec 

def func3():
    print("func3");
    return 1

def func2():
    print("func2");
    return func3

def func1():
    print("func1");
    return func2

func1()()()

