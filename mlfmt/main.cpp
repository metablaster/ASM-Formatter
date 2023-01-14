
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\main.cpp
 *
 * Defines the entry point for application
 *
*/

#include "pch.hpp"
#include "console.hpp"


int main()
{
	if (!RegisterConsoleHandler())
		return -1;

	return 0;
}
