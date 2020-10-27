#pragma once

#include <string>
#include <array>
#include <system_error>

#include <Windows.h>
#include <dbghelp.h>
#pragma comment( lib, "dbghelp" )



namespace vcstackwalker
{
    // definition
    inline std::string GetStackTrace();
    void SetupExceptionHandler();


    LONG CALLBACK _Handler(EXCEPTION_POINTERS* exceptionPointers);
    std::string _Code2Message(unsigned int errCode);
    std::string _GetInfo(EXCEPTION_RECORD* record);
    // end definition


    void SetupExceptionHandler()
    {
        ::SetUnhandledExceptionFilter(_Handler);
    }

    std::string GetStackTrace()
    {
        // initialize
        ::SymInitialize(::GetCurrentProcess(), nullptr, TRUE);

        CONTEXT context;
        STACKFRAME stackFrame = {};

        // context for StackWalk
        RtlCaptureContext(&context);

        // setup stack frame
        stackFrame.AddrPC.Offset = context.Rip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context.Rsp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context.Rsp;
        stackFrame.AddrStack.Mode = AddrModeFlat;

        // symbol buffer
        constexpr size_t BUFFER_SIZE = 1024;
        std::array<byte, BUFFER_SIZE> buffer;
        IMAGEHLP_SYMBOL* symbolBuffer = reinterpret_cast<IMAGEHLP_SYMBOL*>(buffer.data());

        symbolBuffer->SizeOfStruct = BUFFER_SIZE;
        symbolBuffer->MaxNameLength = BUFFER_SIZE - sizeof(IMAGEHLP_SYMBOL);


        std::string result = "";
        while (true)
        {
#ifdef _WIN64

            BOOL walkResult = ::StackWalk(
                IMAGE_FILE_MACHINE_AMD64,   //DWORD MachineType
                ::GetCurrentProcess(),      //__in HANDLE hProcess,
                ::GetCurrentThread(),       //__in HANDLE hThread,
                &stackFrame,                //__inout LPSTACKFRAME StackFrame,
                &context,                   //__inout PVOID ContextRecord,
                nullptr,                    //__in_opt PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
                SymFunctionTableAccess64,   //__in_opt PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
                SymGetModuleBase64,         //__in_opt PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
                nullptr                     //__in_opt PTRANSLATE_ADDRESS_ROUTINE TranslateAddress
            );
#else
            BOOL walkResult = ::StackWalk(
                IMAGE_FILE_MACHINE_I386,    //DWORD MachineType
                ::GetCurrentProcess(),      //__in HANDLE hProcess,
                ::GetCurrentThread(),       //__in HANDLE hThread,
                &stackFrame,                //__inout LPSTACKFRAME StackFrame,
                &context,                   //__inout PVOID ContextRecord,
                nullptr,                    //__in_opt PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
                SymFunctionTableAccess,     //__in_opt PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
                SymGetModuleBase,         //__in_opt PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
                nullptr                     //__in_opt PTRANSLATE_ADDRESS_ROUTINE TranslateAddress
            );
#endif

            if (!walkResult || stackFrame.AddrFrame.Offset == 0) break;


            // get function name
            DWORD64 displacement = 0;
            BOOL getSymResult = ::SymGetSymFromAddr64(::GetCurrentProcess(), stackFrame.AddrPC.Offset, &displacement, symbolBuffer);

            // result
            std::array<char, BUFFER_SIZE * 2> resultBuffer;
            if (getSymResult)
            {
                sprintf_s(resultBuffer.data(), resultBuffer.size(), "0x%08I64x %s() + 0x%I64x\n", stackFrame.AddrPC.Offset, symbolBuffer->Name, displacement);
            }
            else
            {
                sprintf_s(resultBuffer.data(), resultBuffer.size(), "%08I64x, ---", stackFrame.AddrPC.Offset);
            }
            result += std::string(resultBuffer.data());
        }

        // clean up
        ::SymCleanup(::GetCurrentProcess());
        return result;
    }


    LONG CALLBACK _Handler(EXCEPTION_POINTERS* exceptionPointers)
    {
 
        std::string result = _Code2Message(exceptionPointers->ExceptionRecord->ExceptionCode);
        result += _GetInfo(exceptionPointers->ExceptionRecord) + "\n";

        // initialize
        ::SymInitialize(::GetCurrentProcess(), nullptr, TRUE);

        CONTEXT context;
        STACKFRAME stackFrame = {};

        // context for StackWalk
        RtlCaptureContext(&context);

        // setup stack frame
        stackFrame.AddrPC.Offset = context.Rip;
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Offset = context.Rsp;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Offset = context.Rsp;
        stackFrame.AddrStack.Mode = AddrModeFlat;

        // symbol buffer
        constexpr size_t BUFFER_SIZE = 1024;
        std::array<byte, BUFFER_SIZE> buffer;
        IMAGEHLP_SYMBOL* symbolBuffer = reinterpret_cast<IMAGEHLP_SYMBOL*>(buffer.data());

        symbolBuffer->SizeOfStruct = BUFFER_SIZE;
        symbolBuffer->MaxNameLength = BUFFER_SIZE - sizeof(IMAGEHLP_SYMBOL);


        while (true)
        {
#ifdef _WIN64

            BOOL walkResult = ::StackWalk(
                IMAGE_FILE_MACHINE_AMD64,   //DWORD MachineType
                ::GetCurrentProcess(),      //__in HANDLE hProcess,
                ::GetCurrentThread(),       //__in HANDLE hThread,
                &stackFrame,                //__inout LPSTACKFRAME StackFrame,
                &context,                   //__inout PVOID ContextRecord,
                nullptr,                    //__in_opt PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
                SymFunctionTableAccess64,   //__in_opt PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
                SymGetModuleBase64,         //__in_opt PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
                nullptr                     //__in_opt PTRANSLATE_ADDRESS_ROUTINE TranslateAddress
            );
#else
            BOOL walkResult = ::StackWalk(
                IMAGE_FILE_MACHINE_I386,    //DWORD MachineType
                ::GetCurrentProcess(),      //__in HANDLE hProcess,
                ::GetCurrentThread(),       //__in HANDLE hThread,
                &stackFrame,                //__inout LPSTACKFRAME StackFrame,
                &context,                   //__inout PVOID ContextRecord,
                nullptr,                    //__in_opt PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
                SymFunctionTableAccess,     //__in_opt PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
                SymGetModuleBase,         //__in_opt PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
                nullptr                     //__in_opt PTRANSLATE_ADDRESS_ROUTINE TranslateAddress
            );
#endif

            if (!walkResult || stackFrame.AddrFrame.Offset == 0) break;


            // get function name
            DWORD64 displacement = 0;
            BOOL getSymResult = ::SymGetSymFromAddr64(::GetCurrentProcess(), stackFrame.AddrPC.Offset, &displacement, symbolBuffer);

            // result
            std::array<char, BUFFER_SIZE * 2> resultBuffer;
            if (getSymResult)
            {
                sprintf_s(resultBuffer.data(), resultBuffer.size(), "\t0x%08I64x %s() + 0x%I64x\n", stackFrame.AddrPC.Offset, symbolBuffer->Name, displacement);
            }
            else
            {
                sprintf_s(resultBuffer.data(), resultBuffer.size(), "\t%08I64x, ---", stackFrame.AddrPC.Offset);
            }
            result += std::string(resultBuffer.data());
        }


        // clean up
        ::SymCleanup(::GetCurrentProcess());
        // output
        printf("Trace: %s\n", result.c_str());

        return(EXCEPTION_EXECUTE_HANDLER);
    }




    template<class T> class TraceableException : public T  
    {
    public:
        TraceableException(const char* const message_)
            : T(message_)
            , message(message_)
        {
            message += std::string("\n");
            // initialize
            ::SymInitialize(::GetCurrentProcess(), nullptr, TRUE);

            CONTEXT context;
            STACKFRAME stackFrame = {};

            // context for StackWalk
            RtlCaptureContext(&context);

            // setup stack frame
            stackFrame.AddrPC.Offset = context.Rip;
            stackFrame.AddrPC.Mode = AddrModeFlat;
            stackFrame.AddrFrame.Offset = context.Rsp;
            stackFrame.AddrFrame.Mode = AddrModeFlat;
            stackFrame.AddrStack.Offset = context.Rsp;
            stackFrame.AddrStack.Mode = AddrModeFlat;

            // symbol buffer
            constexpr size_t BUFFER_SIZE = 1024;
            std::array<byte, BUFFER_SIZE> buffer;
            IMAGEHLP_SYMBOL* symbolBuffer = reinterpret_cast<IMAGEHLP_SYMBOL*>(buffer.data());

            symbolBuffer->SizeOfStruct = BUFFER_SIZE;
            symbolBuffer->MaxNameLength = BUFFER_SIZE - sizeof(IMAGEHLP_SYMBOL);


            while (true)
            {
#ifdef _WIN64

                BOOL walkResult = ::StackWalk(
                    IMAGE_FILE_MACHINE_AMD64,   //DWORD MachineType
                    ::GetCurrentProcess(),      //__in HANDLE hProcess,
                    ::GetCurrentThread(),       //__in HANDLE hThread,
                    &stackFrame,                //__inout LPSTACKFRAME StackFrame,
                    &context,                   //__inout PVOID ContextRecord,
                    nullptr,                    //__in_opt PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
                    SymFunctionTableAccess64,   //__in_opt PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
                    SymGetModuleBase64,         //__in_opt PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
                    nullptr                     //__in_opt PTRANSLATE_ADDRESS_ROUTINE TranslateAddress
                );
#else
                BOOL walkResult = ::StackWalk(
                    IMAGE_FILE_MACHINE_I386,    //DWORD MachineType
                    ::GetCurrentProcess(),      //__in HANDLE hProcess,
                    ::GetCurrentThread(),       //__in HANDLE hThread,
                    &stackFrame,                //__inout LPSTACKFRAME StackFrame,
                    &context,                   //__inout PVOID ContextRecord,
                    nullptr,                    //__in_opt PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
                    SymFunctionTableAccess,     //__in_opt PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
                    SymGetModuleBase,         //__in_opt PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
                    nullptr                     //__in_opt PTRANSLATE_ADDRESS_ROUTINE TranslateAddress
                );
#endif

                if (!walkResult || stackFrame.AddrFrame.Offset == 0) break;


                // get function name
                DWORD64 displacement = 0;
                BOOL getSymResult = ::SymGetSymFromAddr64(::GetCurrentProcess(), stackFrame.AddrPC.Offset, &displacement, symbolBuffer);

                // result
                std::array<char, BUFFER_SIZE * 2> resultBuffer;
                if (getSymResult)
                {
                    sprintf_s(resultBuffer.data(), resultBuffer.size(), "\t0x%08I64x %s() + 0x%I64x\n", stackFrame.AddrPC.Offset, symbolBuffer->Name, displacement);
                }
                else
                {
                    sprintf_s(resultBuffer.data(), resultBuffer.size(), "\t%08I64x, ---", stackFrame.AddrPC.Offset);
                }
                message += std::string(resultBuffer.data());
            }

            // clean up
            ::SymCleanup(::GetCurrentProcess());
        }
        ~TraceableException()
        {
            ::SymCleanup(::GetCurrentProcess());
        }
        const char* what() const override
        {
            return this->message.c_str();
        }
    private:
        std::string message;
    };



    std::string _Code2Message(unsigned int rawCode)
    {
        unsigned int code = 0;
        if ((rawCode & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0))
        {
            code = HRESULT_CODE(rawCode);
        }
        else
        {
            code = rawCode;
        }

        std::string message = std::system_category().message(code);
        std::array<char, 1024> buf;
        sprintf_s(buf.data(), buf.size(), "%s (0x%08X)", message.c_str(), code);
        return std::string(buf.data());
    }
    
    std::string _GetInfo(EXCEPTION_RECORD* record)
    {
        std::array<char, 1024> buf;
        sprintf_s(buf.data(), buf.size(), "[address: 0x%08I64x]", (uint64_t)record->ExceptionAddress);
        return std::string(buf.data());
    }

} // end namespace
