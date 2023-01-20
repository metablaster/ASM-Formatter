
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\SourceFile.cpp
 *
 * File read\write function definitions
 * TODO: Testing needed with better samples
 * TODO: CRT error handling is not well implemented, ex. override invalid parameter handler,
 * see docs for function used in this source
 *
*/

#include "pch.hpp"
#include "SourceFile.hpp"
#include "error.hpp"
#include "ErrorCode.hpp"
using namespace wsl;


std::size_t GetFileBytesCount(const std::filesystem::path& file_path)
{
	struct _stat fileinfo{ 0 };
	// MSDN: Returns 0 if the file-status information is obtained
	int status = _stat(file_path.string().c_str(), &fileinfo);

	if (status != 0)
	{
		ShowError(ErrorCode::FunctionFailed, "Failed to get file size of " + file_path.string());
	}

	// MSDN: Size of the file in bytes
	return fileinfo.st_size;
}

std::wstringstream LoadFileW(const std::filesystem::path& file_path, const Encoding& encoding)
{
	FILE* file = NULL;
	std::wstring buffer;

	// MSDN: Opens for reading. If the file doesn't exist or can't be found, the fopen_s call fails.
	std::string mode = "r";

	switch (encoding)
	{
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
		ShowError(ErrorCode::UnsuportedOperation, "Encoding not supported by LoadFileW");
		return std::wstringstream();
	}

	_set_errno(0);
	// MSDN: The fopen_s doesn't open a file for sharing
	// the byte order mark (BOM), if present in the file, determines the encoding.
	// The BOM encoding takes precedence over the encoding that's specified by the ccs flag
	// Files that are opened for writing in Unicode mode have a BOM written to them automatically
	const errno_t status = fopen_s(&file, file_path.string().c_str(), mode.c_str());

	if (status == 0)
	{
		const std::size_t filesize = GetFileBytesCount(file_path);

		// Read entire file contents in to memory
		if (filesize > 0)
		{
			buffer.resize(filesize);

			// MSDN: The fread function reads up to count items of size bytes from the input stream
			// fread returns the number of full items the function read, which may be less than count if an error occurs,
			// or if it encounters the end of the file before reaching count
			const std::size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, file);
			buffer.resize(wchars_read);
			buffer.shrink_to_fit();
		}

		// MSDN: fclose returns 0 if the stream is successfully closed.
		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file" + file_path.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, std::string("Failed to read file ") + file_path.string()), ERROR_INFO);
	}

	return std::wstringstream(buffer);
}

std::stringstream LoadFileA(const std::filesystem::path& file_path)
{
	FILE* file = NULL;
	std::string buffer;

	_set_errno(0);
	const errno_t status = fopen_s(&file, file_path.string().c_str(), "r");

	if (status == 0)
	{
		const std::size_t filesize = GetFileBytesCount(file_path);

		if (filesize > 0)
		{
			buffer.resize(filesize);
			const std::size_t chars_read = fread(&(buffer.front()), sizeof(char), filesize, file);

			buffer.resize(chars_read);
			buffer.shrink_to_fit();
		}

		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file" + file_path.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, std::string("Failed to read file ") + file_path.string()), ERROR_INFO);
	}

	return std::stringstream(buffer);
}

void WriteFileW(const std::filesystem::path& file_path, const std::wstringstream& filedata, const Encoding& encoding)
{
	FILE* file = NULL;

	// MSDN: Opens an empty file for writing. If the given file exists, its contents are destroyed.
	std::string mode = "w";

	switch (encoding)
	{
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
		ShowError(ErrorCode::UnsuportedOperation, "Encoding not supported by WriteFileW");
		return;
	}

	_set_errno(0);
	const errno_t status = fopen_s(&file, file_path.string().c_str(), mode.c_str());

	if (status == 0)
	{
		// MSDN: fwrite returns the number of full items the function writes, which may be less than count if an error occurs
		fwrite(filedata.str().c_str(), sizeof(wchar_t), filedata.str().size(), file);

		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file" + file_path.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, std::string("Failed to write file ") + file_path.string()), ERROR_INFO);
	}
}

void WriteFileA(const std::filesystem::path& file_path, const std::stringstream& filedata)
{
	FILE* file = NULL;

	_set_errno(0);
	const errno_t status = fopen_s(&file, file_path.string().c_str(), "w");

	if (status == 0)
	{
		fwrite(filedata.str().c_str(), sizeof(char), filedata.str().size(), file);

		if (fclose(file) != 0)
		{
			ShowError(ErrorCode::FunctionFailed, "Failed to close file" + file_path.string());
		}
	}
	else
	{
		ShowCrtError(Exception(ErrorCode::FunctionFailed, std::string("Failed to write file ") + file_path.string()), ERROR_INFO);
	}
}
