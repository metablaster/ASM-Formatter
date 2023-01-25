
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\error.hpp
 *
 * Error handling function declarations
 *
*/

#pragma once
#include <memory>
#include <array>
#include <type_traits>
#include <Windows.h>
#include "exception.hpp"
#include "utils.hpp"
#include "ErrorMacros.hpp"
#include "ErrorCode.hpp"


extern std::pair<UINT, UINT> default_CP;

namespace wsl
{
	//
	// Integrate our custom exception class with std
	//
	template<typename>
	struct is_cutom_exception :
		public std::false_type
	{
	};

	template<>
	struct is_cutom_exception<Exception> :
		public std::true_type
	{
	};

	/* error message buffer size used by ForamtMessage API **/
	constexpr short msg_buff_size = 512;

	/**
	 * @brief				Converts an error code to human readable string
	 * @param error_code	Error code which to format
	 * @param dwChars		Number of TCHARs returned, excluding the terminating null character, 0 if failed.
	*/
	[[nodiscard]] const std::shared_ptr<std::array<char, msg_buff_size>>
		FormatErrorMessageA(const DWORD& error_code, DWORD& dwChars);

	/** Helper method to extract info if any from std exception object */
	[[nodiscard]] std::string GetStdExceptionInfo(const std::exception& refStdExcept);

	/** Helper method to extract error only message from std exception object */
	[[nodiscard]] std::string GetStdExceptionMessage(const std::exception& refStdExcept);

	/** Generate error message title depending on flags */
	[[nodiscard]] std::string GenerateErrorTitle(const long& flags);

	/** Ask user how to deal with an error */
	[[nodiscard]] void GetUserResponse(
		const std::string& error_title,
		std::string& error_message,
		const std::string& error_info,
		const DWORD& error_code,
		const long& flags);

	/**
	 * Format error code into a string and show in message box or Console.
	 * COM and Win32 errors
	 *
	 * NOTE: use ERROR_INFO macro to fill first 3 parameters, or ERR_INFO_HR
	 * to fill first 4 parameters to default values
	 *
	 * @param file_name		source code file name, use FILE_NAME macro for this
	 * @param func_namce	name of the function where error occurred
	 * @param line			line number in source file where error happened
	 * @param hr			HRESULT value which will be formated to string
	 * @param info			custom string to show in error message
	 * @param flags			sound of error message and in case of Window app,
	 *						message box appearance
	 * @return				returns raw error code, the code depends on type of error
	*/
	void ShowErrorA(
		PCSTR file_name,
		PCSTR func_name,
		int line,
		HRESULT hr = S_OK,
		PCSTR info = "",
		long flags = MB_ICONERROR);

	/**
	 * Show CRT error in human readable text + additional information.
	 * Arguments are the same as for ShowError, use ERROR_INFO to fill in arguments
	*/
	void ShowCrtErrorA(
		class Exception exception,
		PCSTR file_name,
		PCSTR func_name,
		int line,
		long flags = MB_ICONERROR);

	/**
	 * Show error message from exception objects in MessageBox or Console.
	 * Accepts std exceptions and custom Exception class
	 *
	 * @param exception		exception object
	 * @param file_name		source code file name, use FILE_NAME macro for this
	 * @param func_namce	name of the function where error occurred
	 * @param line			source file line number, use __LINE__ macro for this
	 * @param flags			sound of error message and in case of Window app,
	 *						message box appearance
	 * @return				returns raw error code, the code depends on type of error
	*/
	template <typename ExceptionClass>
	requires std::is_base_of_v<std::exception, ExceptionClass>
	void ShowErrorA(
		ExceptionClass exception,
		PCSTR file_name,
		PCSTR func_name,
		int line,
		long flags = MB_ICONERROR) try
	{
		std::string error_title = GenerateErrorTitle(flags);
		std::string error_message("File:\t\t");

		error_message.append(file_name);
		error_message.append("\r\nFunction:\t");
		error_message.append(func_name);
		error_message.append("\r\nLine:\t\t");
		error_message.append(std::to_string(line));
		error_message.append("\r\nCategory:\t");

		DWORD error_code{ };

		if constexpr (HasCodeMethod<ExceptionClass, const std::error_code & ()>::value)
		{
			// TODO: needs reconsideration
			// if we don't cast, it won't compile in case if ExceptionClass
			// is std exception class, otherwise cast is valid.
			const ExceptionClass& ref = dynamic_cast<ExceptionClass&>(exception);
			const std::error_code& ref_code = ref.code();
			error_code = static_cast<DWORD>(ref_code.value());

			if (!error_code)
			{
				error_title = "No error";
				flags = MB_ICONINFORMATION;
			}

			error_message.append(ref_code.default_error_condition().message());
		}
		else
		{
			// This will be the case when one of the std::exception derivates is casted to std::exception
			// Neither error code nor category can be determined
			error_code = static_cast<DWORD>(wsl::ErrorCode::UnspecifiedError);
			error_message.append("Unable to determine category");
		}

		std::string error_info{ };
		error_message.append("\r\nError:\t\t");

		if constexpr (is_cutom_exception<ExceptionClass>::value)
		{
			const Exception& ref = dynamic_cast<Exception&>(exception);
			error_message.append(ref.what());

			std::string info_data = ref.GetInfo();
			if (!info_data.empty())
			{
				error_info.append(info_data);
			}
		}
		else
		{
			std::string info_data = GetStdExceptionInfo(exception);

			if (info_data.empty())
			{
				error_message.append(exception.what());
			}
			else
			{
				error_message.append(GetStdExceptionMessage(exception));
				error_info.append(info_data);
			}
		}

		// Format message adds new line (for WIN32 errors)
		// for exceptions and HRESULTS we add manually
		// so for consistency we add it for exception errors too.
		error_message.append("\r\n");
		GetUserResponse(error_title, error_message, error_info, error_code, flags);
	}
	catch (...)
	{
		// Restore modified console code page and exit program
		// Using SetConsoleCodePage() would produce infinite loop if code page is invalid
		SetConsoleCP(default_CP.first);
		SetConsoleOutputCP(default_CP.second);

		// TODO: For exception free ShowError custom string class is needed that works on stack with
		// only purpose to be used for customized ShowError function.
		MessageBoxA(nullptr, "Likely string constructor exception", "Exception in ShowError", MB_ICONERROR);
		std::exit(wsl::ExitCode(ErrorCode::UnspecifiedError));
	}
}
