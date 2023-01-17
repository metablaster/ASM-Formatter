
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\utils.cpp
 *
 * Utility function definitions
 *
*/

#include "pch.hpp"
#include "utils.hpp"


namespace wsl
{
	bool IsConsole() noexcept
	{
		std::array<DWORD, 1> processes;

		// Retrieves a list of the processes attached to the current console.
		// if it's windows program the return value is zero,
		// otherwise the return value is number of processes attached to the console
		const DWORD process_list = GetConsoleProcessList(processes.data(), static_cast<DWORD>(processes.size()));

		return static_cast<bool>(process_list);
	}
}
