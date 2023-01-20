
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\exception.hpp
 *
 * Declaration of custom exception class
 *
*/

#pragma once
#include <stdexcept>
#include "ErrorMacros.hpp"


namespace wsl
{
	/*
	* Custom exception class used to pass into ShowError function
	* it's interface provides basic functions to retrieve more
	* information about the error than what the standard exception class provides.
	*/
	#pragma warning (disable : 4275)	// base needs to have DLL interface
	class Exception :
		public std::runtime_error
	{
		//
		// Constructors
		//
	public:
		~Exception() noexcept;	// cant be inlined in release build

		/** Construct from error enum */
		template<typename Enum>
		Exception(Enum err_enum);

		/** Construct from error enum and string*/
		template<typename Enum>
		Exception(Enum err_enum, std::string message);

		/** Construct from error_code object */
		inline Exception(std::error_code err_code);

		/** Construct from error_code object and string */
		inline Exception(std::error_code err_code, std::string message);

		/** Copy constructor, needed because default implicitly deleted */
		Exception(const Exception& other);	// can't be inline

		/** Move constructor, for consistency with copy ctor */
		Exception(Exception&& other) noexcept(false);	// can't be inline

		//
		// Operators
		//

		/** Copy assignment */
		Exception& operator=(const Exception&) noexcept = default;

		/** Move assignment */
		Exception& operator=(Exception&&) noexcept(false) = default;

		//
		// Ungrouped methods
		//

		/** Append informational message to exception */
		inline void AddInfo(std::string info);

		/** Get error_condidtion name */
		[[nodiscard]] virtual std::string ConditionName() const;

		/** Get error_category name */
		[[nodiscard]] virtual std::string CategoryName() const;

		/** Get error_condition value */
		[[nodiscard]] inline virtual int ConditionValue() const noexcept;

		/** Get error_code value */
		[[nodiscard]] inline virtual int ErrorValue() const noexcept;

		/** Get additional information string passed to constructor */
		[[nodiscard]] inline virtual const std::string& GetInfo() const noexcept;

		/** Get error_code object associated with this exception object */
		[[nodiscard]] inline virtual const std::error_code& code() const noexcept;

		//
		// Members
		//
	private:
		SUPPRESS(4251);		// member needs to have DLL interface
		std::error_code mCode;

		SUPPRESS(4251);		// member needs to have DLL interface
		std::string mInfo;
	};
	#pragma warning (default : 4275)	// base needs to have DLL interface

	template<typename Enum>
	Exception::Exception(Enum err_enum) :
		runtime_error(std::error_code(err_enum).message()),
		mCode(std::error_code(err_enum)),
		mInfo("")
	{
	}

	template<typename Enum>
	Exception::Exception(Enum err_enum, std::string message) :
		runtime_error(std::error_code(err_enum).message()),
		mCode(std::error_code(err_enum)),
		mInfo(message)
	{
	}

	Exception::Exception(std::error_code err_code) :
		runtime_error(err_code.message()),
		mCode(err_code),
		mInfo("")
	{
	}

	Exception::Exception(std::error_code err_code, std::string message) :
		runtime_error(err_code.message()),
		mCode(err_code),
		mInfo(message)
	{
	}

	int Exception::ConditionValue() const noexcept
	{
		return mCode.default_error_condition().value();
	}

	int wsl::Exception::ErrorValue() const noexcept
	{
		return mCode.value();
	}

	const std::string& Exception::GetInfo() const noexcept
	{
		return mInfo;
	}

	const std::error_code& Exception::code() const noexcept
	{
		return mCode;
	}

	void Exception::AddInfo(std::string info)
	{
		mInfo.append(info);
	}
}
