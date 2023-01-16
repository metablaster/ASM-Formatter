
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
*/

/**
 * @file mlfmt\pch.hpp
 *
 * This is a precompiled header file.
 * Files listed below are compiled only once, improving build performance for future builds.
 * This also affects IntelliSense performance, including code completion and many code browsing features.
 * However, files listed here are ALL re-compiled if any one of them is updated between builds.
 * Do not add files here that you will be updating frequently as this negates the performance advantage.
*/

#pragma once
// Solution wide includes
#include "targetver.hpp"
#include "pragmas.hpp"

// Windows Header Files
#include <Windows.h>

// C++ Standard Header Files
#include <iostream>
#include <filesystem>
#include <memory>
#include <array>
#include <string>
#include <sstream>
#include <cuchar>
#include <regex>
#include <bit>

// Restore warnings disabled for precompiled header
#pragma warning (pop)

// Project specific pragmas
