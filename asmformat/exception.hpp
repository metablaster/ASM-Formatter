
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
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


namespace wsl
{
	/**
	 * Custom exception class used to pass into ShowError function.
	 * The interface provides basic functions to retrieve more
	 * information about the error than what the standard exception class provides.
	*/
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
		// Class interface
		//

		/** Append additional informational message to exception */
		void AddInfo(std::string info);

		/** Returns the message describing the error */
		[[nodiscard]] virtual std::string ErrorMessage() const;

		/** Returns the message describing the error condition */
		[[nodiscard]] virtual std::string ConditionMessage() const;

		/** Returns name of error_category associated with error_code */
		[[nodiscard]] virtual std::string ErrorCategoryName() const;

		/** Returns name of error_category associated with error_condition */
		[[nodiscard]] virtual std::string ConditionCategoryName() const;

		/** Returns the error value associated with the error_code object */
		[[nodiscard]] inline virtual int ErrorValue() const noexcept;

		/** Returns the value associated with the error_condition object */
		[[nodiscard]] inline virtual int ConditionValue() const noexcept;

		/** Returns error_code object associated with this exception object */
		[[nodiscard]] inline virtual const std::error_code& code() const noexcept;

		/** Returns additional information string passed to constructor */
		[[nodiscard]] inline virtual const std::string& GetInfo() const noexcept;

		//
		// Members
		//
	private:
		// Object describing the error
		std::error_code mCode;

		// Additional information
		std::string mInfo;
	};

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

	int wsl::Exception::ErrorValue() const noexcept
	{
		return mCode.value();
	}

	int Exception::ConditionValue() const noexcept
	{
		return mCode.default_error_condition().value();
	}

	const std::error_code& Exception::code() const noexcept
	{
		return mCode;
	}

	const std::string& Exception::GetInfo() const noexcept
	{
		return mInfo;
	}
}
