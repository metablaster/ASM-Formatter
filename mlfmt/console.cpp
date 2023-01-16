
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\console.cpp
 *
 * Function definitions specific for console projects
 *
*/

#include "pch.hpp"
#include "console.hpp"


BOOL WINAPI HandlerRoutine(DWORD signal) noexcept
{
	switch (signal)
	{
	case CTRL_C_EVENT:
		ExitProcess(0);
	case CTRL_BREAK_EVENT:
		break;
	case CTRL_CLOSE_EVENT:
		ExitProcess(0);
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		break;
	default:
		break;
	}

	return TRUE;
}

bool RegisterConsoleHandler()
{
	if (!SetConsoleCtrlHandler(HandlerRoutine, TRUE))
	{
		std::wcerr << L"FunctionFailed, Could not set console handler" << std::endl;
		return false;
	}

	return true;
}

bool SetProjectConsole(DWORD code_page)
{
	bool failed = IsValidCodePage(code_page) == 0;

	if (failed)
	{
		std::wcerr << L"FunctionFailed, Code page is not valid" << std::endl;
		return false;
	}

	// set console output and input code page
	failed = SetConsoleOutputCP(code_page) == 0;

	if (failed)
	{
		std::wcerr << L"FunctionFailed, Failed to set console output code page" << std::endl;
	}

	if (SetConsoleCP(code_page) == 0)
	{
		failed = true;
		std::wcerr << L"FunctionFailed, Failed to set console input code page" << std::endl;
	}

	return !failed;
}
