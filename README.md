# vc_stacktrace_sample
visual c++ stack trace sample program

```c++
#include <cstdio>
#include <stdexcept>

#include "stacktrace.h"

// for Structured exception handling
void SE_F1();
void SE_F2();
void SE_F3();

void SE_F1()
{
    SE_F2();
}
void SE_F2()
{
    SE_F3();
}
void SE_F3()
{
    auto zero = 0;
    auto n = 2 / zero;
}

// for c++ exception
void Ex_F1();
void Ex_F2();
void Ex_F3();

void Ex_F1()
{
    Ex_F2();
}
void Ex_F2()
{
    Ex_F3();
}
void Ex_F3()
{
    throw vcstackwalker::TraceableException<std::runtime_error>("Ex_F3 Throw");
}

int main()
{

    printf("test c++ exception ---- ---- ---- ----\n");
    try
    {
        Ex_F1();
    }
    catch (std::runtime_error& e)
    {
        printf("catch c++ runtime_error\n");
        printf("%s\n", e.what());
    }
    printf("\n");

    printf("test c++ exception ---- ---- ---- ----\n");
    vcstackwalker::SetupExceptionHandler();
    SE_F1();
}

```

```
test c++ exception ---- ---- ---- ----
catch c++ runtime_error
Ex_F3 Throw
        0x7ff71f0437d5 vcstackwalker::TraceableException<std::runtime_error>::TraceableException<std::runtime_error>() + 0x115
        0x7ff71f045c2d Ex_F3() + 0x3d
        0x7ff71f045bcf Ex_F2() + 0x2f
        0x7ff71f045b7f Ex_F1() + 0x2f
        0x7ff71f048f2c main() + 0x3c
        0x7ff71f049e49 invoke_main() + 0x39
        0x7ff71f049cee __scrt_common_main_seh() + 0x12e
        0x7ff71f049bae __scrt_common_main() + 0xe
        0x7ff71f049ed9 mainCRTStartup() + 0x9
        0x7ffb82247034 BaseThreadInitThunk() + 0x14
        0x7ffb82adcec1 RtlUserThreadStart() + 0x21


test c++ exception ---- ---- ---- ----
Trace: unknown error (0xC0000094)[address: 0x7ff71f045d37]
        0x7ff71f046a4b vcstackwalker::_Handler() + 0x11b
        0x7ffb8031bb47 UnhandledExceptionFilter() + 0x1e7
        0x7ffb82b34973 memset() + 0x1b33
        0x7ffb82b1b8f6 _C_specific_handler() + 0x96
        0x7ffb82b310ef _chkstk() + 0x11f
        0x7ffb82adb474 RtlRaiseException() + 0x434
        0x7ffb82b2fc1e KiUserExceptionDispatcher() + 0x2e
        0x7ff71f045d37 SE_F3() + 0x37
        0x7ff71f045cdf SE_F2() + 0x2f
        0x7ff71f045c8f SE_F1() + 0x2f
        0x7ff71f048f51 main() + 0x61
        0x7ff71f049e49 invoke_main() + 0x39
        0x7ff71f049cee __scrt_common_main_seh() + 0x12e
        0x7ff71f049bae __scrt_common_main() + 0xe
        0x7ff71f049ed9 mainCRTStartup() + 0x9
        0x7ffb82247034 BaseThreadInitThunk() + 0x14
        0x7ffb82adcec1 RtlUserThreadStart() + 0x21
```
