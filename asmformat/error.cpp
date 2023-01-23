
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\error.cpp
 *
 * Error handling function definitions
 *
*/

#include "pch.hpp"
#include "error.hpp"
#include "ErrorCode.hpp"


namespace wsl
{
	const std::shared_ptr<std::array<char, msg_buff_size>>
		FormatErrorMessage(const DWORD& error_code, DWORD& dwChars)
	{
		const std::shared_ptr<std::array<char, msg_buff_size>> message =
			std::make_shared<std::array<char, msg_buff_size>>();

		dwChars = FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,	// The location of the message definition.
			error_code,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			message.get()->data(),
			msg_buff_size,
			nullptr);

		return message;
	}

	// TODO: limit prompt to OK, CANCEL depending on flags,
	// ex: only abort if error.
	void GetUserResponse(
		const std::string& error_title,
		std::string& error_message,
		const std::string& error_info,
		const DWORD& error_code,
		const long& flags)
	{
		int response = 0;
		const bool console = IsConsole();

		if (!error_info.empty())
		{
			// New lined added by FormatMessage or template function
			error_message.append("Info:\t\t");
			error_message.append(error_info);

			// New line only if info and console
			// because if no info new line is already added
			if (console)
				error_message.append("\r\n");
		}

		if (console)
		{
			MessageBeep(static_cast<UINT>(flags));

			std::cerr << std::endl << error_title.c_str() << std::endl;
			std::cerr << error_message.c_str() << std::endl;

			const bool iconstop = flags & MB_ICONSTOP;
			std::string message = "Your response?: OK [Any key]";
			if (iconstop)
				message = "Your response?: Continue [C], Exit [Any key]";

			std::cout << (message += " ");
			std::cin.clear();
			std::cin.ignore(std::cin.gcount());

			std::string input;
			std::getline(std::cin, input);
			std::cout << std::endl;

			if (iconstop)
			{
				if (input.empty())
					response = IDCANCEL; // Enter key
				else response = input.at(0); // First character
			}
			else response = IDOK;
		}
		else
		{
			long result_flags = MB_OK | flags;

			if (flags & MB_ICONSTOP)
				result_flags = MB_OKCANCEL | flags;

			response = MessageBoxA(
				GetActiveWindow(),
				error_message.c_str(),
				error_title.c_str(),
				static_cast<UINT>(result_flags));
		}

		switch (response)
		{
		case 'C':
		case 'c':
		case IDOK:
			return;
		case IDCANCEL:
		default:
			std::exit(static_cast<int>(error_code));
		}
	}

	std::string GenerateErrorTitle(const long& flags)
	{
		std::string error_title("Runtime Error");

		// Welcome to Microsoft non standard behavior!
		// TODO: see if HIWORD OR LOWORD can be used.
		if ((flags & 0xf0u) == MB_ICONWARNING)
		{
			error_title = "Runtime Warning";
		}
		else if ((flags & 0xf0u) == MB_ICONINFORMATION)
		{
			error_title = "Runtime Information";
		}

		return error_title;
	}

	std::string GetStdExceptionInfo(const std::exception& refStdExcept)
	{
		const std::string str = refStdExcept.what();
		const std::size_t pos = str.find(": ");

		if (pos != std::string::npos)
		{
			return std::string(str.substr(0, pos));;
		}

		return std::string();
	}

	std::string GetStdExceptionMessage(const std::exception& refStdExcept)
	{
		const std::string str = refStdExcept.what();
		const std::size_t pos = str.find(": ");

		if (pos != std::string::npos)
		{
			return std::string(str.substr(pos + 2, str.size()));
		}

		return str;
	}

	void ShowErrorA(
		PCSTR file_name,
		PCSTR func_name,
		int line,
		HRESULT hr,
		PCSTR info,
		long flags) noexcept try
	{
		// We need to get error code immediately,
		// ex. std::string below will set it to zero!
		DWORD error_code = GetLastError();

		std::string error_title = GenerateErrorTitle(flags);
		std::string error_message("File:\t\t");

		error_message.append(file_name);
		error_message.append("\r\nFunction:\t");
		error_message.append(func_name);
		error_message.append("\r\nLine:\t\t");
		error_message.append(std::to_string(line));
		error_message.append("\r\nCategory:\t");

		// If HRESULT is omitted or S_OK
		// format last error code message
		if (hr == S_OK)
		{
			error_message.append("Win32 error");
			error_message.append("\r\nError:\t\t");

			if (!error_code)
			{
				error_title = "No error";
				flags = MB_ICONINFORMATION;
			}

			DWORD dwChars = 0;
			auto message = FormatErrorMessage(error_code, dwChars);

			// If the function succeeds, the return value is
			// the number of TCHARs stored in the output buffer,
			// excluding the terminating null
			if (dwChars)
			{
				error_message.append(message.get()->data());
			}
			else // If the function fails, the return value is zero
			{
				error_message.append("FormatErrorMessage failed to translate " + std::to_string(error_code));

				// for some odd reason it will print newline on it's own
				// error_message.append(L"\n");
			}
		}
		else // Format com error code into a message
		{
			error_message.append("COM error");
			error_message.append("\r\nError:\t\t");

			error_code = static_cast<unsigned long>(hr);
			if (SUCCEEDED(error_code))
			{
				error_title = "No error";
				flags = MB_ICONINFORMATION;
			}

			error_message.append(_com_error(hr).ErrorMessage());

			// Format message adds new line (for WIN32 errors)
			// for exceptions and HRESULTS we add manually
			// so for consistency we add it for exception errors too.
			error_message.append("\r\n");
		}

		GetUserResponse(error_title, error_message, info, error_code, flags);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "Likely string constructor exception", "Exception in ShowError", MB_ICONERROR);
	}

	void ShowCrtErrorA(
		Exception exception,
		PCSTR file_name,
		PCSTR func_name,
		int line,
		long flags) noexcept try
	{
		constexpr std::size_t size = 100;

		// we will use SUPPRESS for false warning because
		// MSDN: The error-message string is terminated by the newline character ('\n')
		char crt_error[size];

		// Zero if successful, an error code on failure.
		// If buffer is NULL or if the size parameter is 0, the invalid parameter handler is invoked.
		// If execution is allowed to continue, the functions return EINVAL and set errno to EINVAL
		const errno_t status = strerror_s<size>(crt_error, errno);

		if (status != 0)
		{
			StringCbCopyA(crt_error, size, ("strerror_s failed with " + std::to_string(status) + " to translate " + std::to_string(errno)).c_str());
		}

		std::string error_title = GenerateErrorTitle(flags);
		std::string error_message("File:\t\t");

		error_message.append(file_name);
		error_message.append("\r\nFunction:\t");
		error_message.append(func_name);
		error_message.append("\r\nLine:\t\t");
		error_message.append(std::to_string(line));

		// OK, errno is either zero or positive
		const DWORD error_code = static_cast<DWORD>(errno);

		if (!error_code)
		{
			error_title = "No error";
			flags = MB_ICONINFORMATION;
		}

		error_message.append("\r\nCategory:\t");
		error_message.append(exception.ConditionName());

		error_message.append("\r\nError:\t\t");
		error_message.append(exception.what());
		error_message.append("\r\nCRT Error\t");

		SUPPRESS(6054 26485);	// String might not be zero-terminated and No array to pointer decay
		error_message.append(crt_error);

		std::string error_info{};
		std::string info_data = exception.GetInfo();

		if (!info_data.empty())
			error_info.append(info_data);

		// Format message adds new line (for WIN32 errors)
		// for exceptions and HRESULTS we add manually
		// so for consistency we add it for exception errors too.
		error_message.append("\r\n");
		GetUserResponse(error_title, error_message, error_info, error_code, flags);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "Likely string constructor exception", "Exception in ShowError", MB_ICONERROR);
	}
}
