
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 * 
 * NOTE: Portions of code in this file are not MIT but licensed under their own terms,
 * for more information see individual comments per function
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

	void ReplaceAll(std::string& source, const std::string& from, const std::string& to)
	{
		// AUTHOR:
		// https://stackoverflow.com/a/29752943/12091999
		// LICENSE:
		// https://creativecommons.org/licenses/by-sa/3.0
		// CHANGES:
		// variables casing

		std::string new_string;
		new_string.reserve(source.length());

		std::string::size_type last_pos = 0;
		std::string::size_type find_pos;

		while (std::string::npos != (find_pos = source.find(from, last_pos)))
		{
			new_string.append(source, last_pos, find_pos - last_pos);
			new_string += to;
			last_pos = find_pos + from.length();
		}

		// Care for the rest after last occurrence
		new_string += source.substr(last_pos);

		source.swap(new_string);
	}
}
