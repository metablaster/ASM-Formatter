
/*
 *	Project: "masm-formatter" https://github.com/metablaster/masm-formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\SourceFile.cpp
 *
 * Read\Write function definitions
 *
*/

#include "pch.hpp"
#include "SourceFile.hpp"


std::size_t GetFileSize(const std::filesystem::path& file_path)
{
	struct _stat fileinfo;
	_stat(file_path.string().c_str(), &fileinfo);
	return fileinfo.st_size;
}

std::wstringstream LoadFileW(const std::filesystem::path& file_path, const Encoding& encoding)
{
	FILE* file;
	std::wstring buffer;
	// Opens for both reading and writing. The file must exist.
	std::string mode = "r";

	switch (encoding)
	{
	case Encoding::ANSI:
		std::cerr << "Encoding not supported by LoadFileW";
		return std::wstringstream();
	case Encoding::UTF8:
		mode += ", ccs=UTF-8";
		break;
	case Encoding::UTF16:
		mode += ", ccs=UNICODE";
		break;
	case Encoding::UTF16LE:
		mode += ", ccs=UTF-16LE";
		break;
	default:
		break;
	}

	const errno_t err = fopen_s(&file, file_path.string().c_str(), mode.c_str());

	if (err == 0)
	{
		std::size_t filesize = GetFileSize(file_path);

		// Read entire file contents in to memory
		if (filesize > 0)
		{
			buffer.resize(filesize);
			const std::size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, file);
			buffer.resize(wchars_read);
			buffer.shrink_to_fit();
		}

		fclose(file);
	}

	// ...handle some error...
	return std::wstringstream(buffer);
}

std::stringstream LoadFileA(const std::filesystem::path& file_path)
{
	FILE* file;
	std::string buffer;
	// Opens for both reading and writing. The file must exist.
	const errno_t err = fopen_s(&file, file_path.string().c_str(), "r");

	if (err == 0)
	{
		std::size_t filesize = GetFileSize(file_path);

		// Read entire file contents in to memory
		if (filesize > 0)
		{
			buffer.resize(filesize);
			const std::size_t wchars_read = fread(&(buffer.front()), sizeof(char), filesize, file);
			buffer.resize(wchars_read);
			buffer.shrink_to_fit();
		}

		fclose(file);
	}

	return std::stringstream(buffer);
}

void WriteFileW(const std::filesystem::path& file_path, const std::wstringstream& filedata, const Encoding& encoding)
{
	FILE* file;
	std::string mode = "w";

	switch (encoding)
	{
	case Encoding::ANSI:
		std::cerr << "Encoding not supported by WriteFileW";
		return;
	case Encoding::UTF8:
		mode += ", ccs=UTF-8";
		break;
	case Encoding::UTF16:
		mode += ", ccs=UNICODE";
		break;
	case Encoding::UTF16LE:
		mode += ", ccs=UTF-16LE";
		break;
	default:
		break;
	}

	const errno_t err = fopen_s(&file, file_path.string().c_str(), mode.c_str());

	if (err == 0)
	{
		fwrite(filedata.str().c_str(), sizeof(wchar_t), filedata.str().size(), file);
		fclose(file);
	}
}

void WriteFileA(const std::filesystem::path& file_path, const std::stringstream& filedata)
{
	FILE* file;
	const errno_t err = fopen_s(&file, file_path.string().c_str(), "w");

	if (err == 0)
	{
		fwrite(filedata.str().c_str(), sizeof(char), filedata.str().size(), file);
		fclose(file);
	}
}
