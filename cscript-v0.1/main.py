f = __import__('func', None, None, [], 0)
print(f.FUNC)

a = f.func()
a[2] = 0;
a = f.func()
print(a)

l = locals()
ret = l.get("a")
print(ret)
