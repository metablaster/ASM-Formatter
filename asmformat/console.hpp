
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\console.hpp
 *
 * Function declarations specific for console projects
 *
*/

#pragma once
#include <Windows.h>


/**
 * An application-defined function used with the SetConsoleCtrlHandler function.
 * A console process uses this function to handle control signals received by the process.
 * When the signal is received, the system creates a new thread in the process to execute the function.
*/
BOOL WINAPI HandlerRoutine(DWORD signal) noexcept;

/**
 * Helper function to set console handler
 * NOTE: To make it work, in VS uncheck: Exception Settings -> Win32 Exceptions -> Control-C
*/
bool RegisterConsoleHandler();

/**
 * @brief Set console input and output code page
 * @param code_page code page to set
*/
bool SetProjectConsole(DWORD code_page);
