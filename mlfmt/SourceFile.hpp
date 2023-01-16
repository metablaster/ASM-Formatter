
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\SourceFile.hpp
 *
 * Read\Write function declarations
 *
*/

#pragma once
#include <filesystem>
#include <sstream>


enum class Encoding
{
	ANSI,
	UTF8,
	UTF16,
	UTF16LE
};

std::size_t GetFileSize(const std::filesystem::path& file_path);

std::wstringstream LoadFileW(const std::filesystem::path& file_path, const Encoding& encoding);

std::stringstream LoadFileA(const std::filesystem::path& file_path);

void WriteFileW(const std::filesystem::path& file_path, const std::wstringstream& filedata, const Encoding& encoding);

void WriteFileA(const std::filesystem::path& file_path, const std::stringstream& filedata);
