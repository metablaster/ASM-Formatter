
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\console.hpp
 *
 * Function declarations specific for console projects
 * https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
 *
*/

#pragma once
#include <Windows.h>
#include <utility>


namespace wsl
{
	/**
	 * An application-defined function used with the SetConsoleCtrlHandler function.
	 * A console process uses this function to handle control signals received by the process.
	 * When the signal is received, the system creates a new thread in the process to execute the function.
	*/
	BOOL WINAPI HandlerRoutine(DWORD signal) noexcept;

	/**
	 * Helper function to set console handler
	 * NOTE: To make it work during debugging, in VS uncheck: Exception Settings -> Win32 Exceptions -> Control-C
	*/
	bool RegisterConsoleHandler();

	/**
	 * @brief Set console input and output code page
	 * @param input		console input code page which to set
	 * @param output	console output code page which to set
	*/
	bool SetConsoleCodePage(DWORD input, DWORD output);

	/**
	 * @brief Get console input and output code page
	 * @return	Input followed by Output code page pair
	*/
	std::pair<UINT, UINT> GetConsoleCodePage();
}
