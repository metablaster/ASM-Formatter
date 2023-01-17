
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
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
#include <comdef.h>		// _com_error (error.cpp)

// C++ Standard Header Files
#include <iostream>
#include <filesystem>	// std::filesystem::path (SourceFile.hpp)
#include <string>		// std::string
#include <sstream>		// std::stringstream (SourceFile.hpp)
#include <cuchar>		// std::c16rtomb (StringCast.cpp)
#include <regex>		// std::regex_search (FormatFile.cpp)
#include <bit>			// std::endian (utils.hpp)
#include <array>		// std::array (error.hpp)
#include <memory>		// std::shared_ptr (error.hpp)
#include <cwchar>		// std::wcsrchr, std::size_t (ErrorMacros.hpp)
#include <system_error>	// std::is_error_code_enum (ErrorCode.hpp)
#include <utility>		// std::declval, std::move (utils.hpp, exception.cpp)
#include <stdexcept>	// std::runtime_error (exception.hpp)
#include <type_traits>	// std::false_type (error.hpp, utils.hpp)
#include <cstdint>		// std::uint16_t (ErrorCode.hpp)

// C Standard header files
#include <stdio.h>		// fopen_s (SourceFile.cpp)

// Restore warnings disabled for precompiled header
#pragma warning (pop)

// Project specific pragmas