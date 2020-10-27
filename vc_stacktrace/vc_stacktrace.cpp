// vcpp_stacktrace.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

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
