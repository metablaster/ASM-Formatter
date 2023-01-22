
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\console.cpp
 *
 * Function definitions specific for console projects
 *
*/

#include "pch.hpp"
#include "console.hpp"
#include "error.hpp"
#include "ErrorCode.hpp"


namespace wsl
{
	BOOL WINAPI HandlerRoutine(DWORD signal) noexcept
	{
		switch (signal)
		{
		case CTRL_C_EVENT:
			ExitProcess(0u);
		case CTRL_BREAK_EVENT:
			break;
		case CTRL_CLOSE_EVENT:
			ExitProcess(0u);
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			break;
		default:
			break;
		}

		return TRUE;
	}

	bool RegisterConsoleHandler() noexcept(false)
	{
		// MSDN: If the function fails, the return value is zero.
		// To get extended error information, call GetLastError
		if (SetConsoleCtrlHandler(HandlerRoutine, TRUE) == FALSE)
		{
			ShowError(ERROR_INFO_HR, "Could not set console handler");
			return false;
		}

		return true;
	}

	bool SetConsoleCodePage(DWORD input, DWORD output)
	{
		// MSDN: Determines if a specified code page is valid
		// Returns a nonzero value if the code page is valid, or 0 if the code page is invalid
		bool failed = (IsValidCodePage(input) == FALSE) || (IsValidCodePage(output) == FALSE);

		if (failed)
		{
			ShowError(ErrorCode::BadArgument, "Code page is not valid");
			return false;
		}

		// MSDN: Sets the output code page used by the console associated with the calling process
		// If the function succeeds, the return value is nonzero
		failed = SetConsoleOutputCP(output) == FALSE;

		if (failed)
		{
			ShowError(ERROR_INFO_HR, "Failed to set console output code page");
		}

		// MSDN: Sets the input code page used by the console associated with the calling process
		// If the function succeeds, the return value is nonzero
		if (SetConsoleCP(input) == FALSE)
		{
			failed = true;
			ShowError(ERROR_INFO_HR, "Failed to set console input code page");
		}

		return !failed;
	}

	std::pair<UINT, UINT> GetConsoleCodePage() noexcept(false)
	{
		// MSDN: If the return value is zero, the function has failed.
		// To get extended error information, call GetLastError
		const UINT input = GetConsoleCP();
		if (input == 0)
		{
			ShowError(ERROR_INFO);
			return std::make_pair(0u, 0u);
		}

		const UINT output = GetConsoleOutputCP();
		if (output == 0)
		{
			ShowError(ERROR_INFO);
			return std::make_pair(0u, 0u);
		}

		return std::make_pair(input, output);
	}
}
