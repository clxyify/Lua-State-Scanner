# Lua-State-Scanner

Scans for **Lua State** and **Script Context VFT** and therefore creates a thread onto a ```uintptr_t``` variable.

How does this work? Well first it will scan the ```Memory``` for the Script Context VFTable.

The code sampled here uses an operander symbol `=` to match the results onto a new variable.
```cpp
rL = *(std::uintptr_t*)(ScriptContext + 308) - (ScriptContext + 308); /* opperander to scan lua state */
```
As you see `rL` was defined as the `Lua State` + `Script Context` scanned to use between a function.

I did not make this scanner, someone named *asdf* developed it back in 2020- the semi-golden era.

The `rbx_gettop` was developed/modified by fishy-sticks on github. It was used for a test reference.

Sometimes it works, sometimes it doesn't. This was testing and it worked on some of the functions.

Functions must be a ```uintptr_t``` type that returns the result of the value, in std:: terms as well.

# Tutorial

Go to the function `ScriptContextVFT` and only replace the address, dont replace the code after that.

In the injection thread, find `rL =` and replace the code with the new latest `lua state` (can be found in getstate).

**P.S Lua State has not changed since November of 2021. Lua State should work for a little longer until it dies.**

# Lua Terms

Lua does the same exact thing, except it scans with 1 line of `code` without complication code.

```cpp
lua_State L;
L = luaL_newstate();
```
It only takes 1-2 seconds to create the entire thread, and can also be closed with `lua_close(L);`
