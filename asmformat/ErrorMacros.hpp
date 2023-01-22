
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\ErrorMacros.hpp
 *
 * Error macros used for error reporting
 *
*/

#pragma once
#include <cstring>
//#include "error.hpp"
//#include "ErrorCode.hpp"
//#include "exception.hpp"


// Exception class constructor takes std::string which may throw or
// ShowError takes Exception class
#if defined _DEBUG
#define NOEXCEPT noexcept(false)
#else
#define NOEXCEPT noexcept
#endif // _DEBUG

// Useful inline pragmas for warnings
#define PUSH __pragma(warning(push))
#define POP __pragma(warning(pop))
#define SUPPRESS(...) __pragma(warning(suppress : __VA_ARGS__))
#define DISABLE(...) __pragma(warning(disable : __VA_ARGS__))

#if defined _DEBUG || !defined SHOW_ERRORS || !defined USE_LOGGING
// used for empty macro functions to avoid warning for "empty statement found ';' "
#define EMPTY_STATEMENT static_cast<void>(0)
#endif

//
// Macros useful for debugging
//

// Global macros which could be used in build configurations,
// some of these macros must always be defined since some non-ShowError
// functions use them.

// we need this workaround because MSVC is not compliant
#define EXPAND(x) x

// show only file name instead of full path ANSI version
#define FILE_NAME PUSH DISABLE(26485) /* no array to pointer decay */ \
	(std::strrchr(__FILE__, '\\') ? \
	std::strrchr(__FILE__, '\\') + 1 : __FILE__) POP

// string function name
#define FUNC_NAME __FUNCTION__

// Boilerplate macro
#define ERROR_INFO FILE_NAME, FUNC_NAME, __LINE__
#define ERROR_INFO_HR FILE_NAME, FUNC_NAME, __LINE__, S_OK

#if defined SHOW_ERRORS || defined _DEBUG

// ShowError function
#define ERR_FUNC wsl::ShowErrorA
#define ShowCrtError wsl::ShowCrtErrorA

// Expansion macros
#define GET_ERR_FUNC(N1, N2, N3, N4, N5, N6, FUNC, ...) FUNC

// ShowError macro overloads for ShowError
// TODO: first function not good if argument is non custom exception, implicitly converted? if it is them unnecessary copy ctor.
#define ERR_FUNC1(N1) ERR_FUNC(wsl::Exception(N1), ERROR_INFO)	// used to pass enum error code
#define ERR_FUNC2(N1, N2) ERR_FUNC(wsl::Exception(N1, N2), ERROR_INFO)	// used to pass enum error code with additional info string
#define ERR_FUNC3(N1, N2, N3) ERR_FUNC(N1, N2, N3)	// must be file_name, func_name, line
#define ERR_FUNC4(N1, N2, N3, N4) ERR_FUNC(N1, N2, N3, N4)	// depends on N1
#define ERR_FUNC5(N1, N2, N3, N4, N5) ERR_FUNC(N1, N2, N3, N4, N5)	// depends on N1
#define ERR_FUNC6(N1, N2, N3, N4, N5, N6) ERR_FUNC(N1, N2, N3, N4, N5, N6)	// calls plain function

/**
 * @brief show error message either in console if console program otherwise in MessageBoxW
 *
 * 1 argument version accepts error code enum for custom exception class
 * 2 arguments version accepts error code enum for custom exception class + info string
 * 3 arguments version accepts ERROR_INFO macro and shows result of WIN32 error
 * 4 arguments version accepts either ERROR_INFO + HRESULT or any exception class + ERROR_INFO
 * 5 arguments version same as 4 args + in case of exception class messagebox flags used to
 * influence sound and/or MsgBox appearance, an in case if no exception class it accepts additional info string
 * 6 args version accepts non exception args as case above + message box flags as last argument.
 *
 * NOTE: use ERROR_INFO macro to fill first 3 parameters, or ERR_INFO_HR
 * to fill first 4 parameters to default values
 * NOTE: ERROR_INFO macro expands to 3 parameters (filename, function and line) and
 * ERR_INFO_HR in addition sets 4th parameter to S_OK, meaning handling Win32 error
*/
#define ShowError(...) EXPAND(GET_ERR_FUNC(__VA_ARGS__, ERR_FUNC6, ERR_FUNC5, ERR_FUNC4, ERR_FUNC3, ERR_FUNC2, ERR_FUNC1)(__VA_ARGS__))

// TODO: need to revisit where below macros are used, should be used only temporally for debugging

// show message if failed hr
#define CHECK_HR(hresult) if(FAILED(hresult)) { ShowError(ERROR_INFO, hresult); }

// if failed hr return hr
#define CHECK_HR_RETURN(hresult) if(FAILED(hresult)) { ShowError(ERROR_INFO, hresult); return hresult; }

// show message if Win32 error
#define CHECK_WIN32(win32_error) if(win32_error) { ShowError(ERROR_INFO); }

// show message if pointer not valid
#define CHECK_POINTER(ptr) if(ptr == nullptr) { ShowError(wsl::ErrorCode::InvalidPointer); }

#else

// TODO: how to handle these macros in release builds? warnings generated for empty control statement.
// define throw _VA_ARGS_ with the help of macros header, then Exception class needs new constructor to accept ERROR_INFO
// or somehow make use of HandleException

// not used
#define ShowError(...) EMPTY_STATEMENT
// not used
#define ShowCrtError(...) EMPTY_STATEMENT
// not used
#define CHECK_HR(hresult) EMPTY_STATEMENT
// not used
#define CHECK_HR_RETURN(hresult) EMPTY_STATEMENT
// not used
#define CHECK_WIN32(win32_error) EMPTY_STATEMENT
// not used
#define CHECK_POINTER(ptr) EMPTY_STATEMENT

#endif // SHOW_ERRORS | _DEBUG
