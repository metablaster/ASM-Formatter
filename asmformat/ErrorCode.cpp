
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\ErrorCode.cpp
 *
 * Definition of custom error_code
 *
*/

#include "pch.hpp"
#include "ErrorCode.hpp"
#include "ErrorMacros.hpp"
#include "ErrorCondition.hpp"


/*
 * Explanation of std error handling
 *
 * 1. std::system_error		(exception class/object being thrown) takes std::error_code (or individual parts of error_code) for construction
 * 
 * 2. std::error_code		It's used for storing and transmitting error codes as they were produced by originating library, unchanged.
 * 
 * (object explaining the error) takes int error value and std::error_category or custom error enum for it's construction
 * in case of custom error enum 'make_error_code' must be overridden to construct error_code from error_category instance and error value.
 *
 * 3. std::error_category	It's used as a technical detail for implementing custom error_condition and error_code.
 * 
 * should be inherited and only a single instance of that class is used to describe error category,
 * see comment in declaration of error_category below for more info.
 *
 * 4. std::error_condition	It's used for performing queries on error_codes, for the purpose of grouping or classification or translation.
 * 
 * Stores an abstract error code and category.
 * Objects of this type describe errors in a generic way so that they may be portable across different systems.
 * This is in contrast with error_code objects, that may contain system-specific information.
 * error_condition objects are generally used to check whether a particular error_code obtained from the system matches a specific
 * error condition no matter the system.
*/

namespace wsl
{
	/**
	 * Category types are used to identify the source of an error.
	 * They also define the relation between error_code and error_condition objects of its category,
	 * as well as the message set for error_code objects.
	*/
	class ErrorCodeCategory :
		public std::error_category
	{
		//
		// Constructors
		//
	public:
		~ErrorCodeCategory() = default;
		ErrorCodeCategory() = default;

	private:
		/*
		 * Objects of these types have no distinct values and are not-copyable and not-assignable,
		 * and thus can only be passed by reference.
		 * As such, only one object of each of these types shall exist, each uniquely identifying its own category:
		 * all error codes and conditions of a same category shall return a reference to same object.
		*/
		ErrorCodeCategory(const ErrorCodeCategory&) = delete;
		ErrorCodeCategory(ErrorCodeCategory&&) = delete;

		//
		// Operators
		//
		ErrorCodeCategory& operator=(const ErrorCodeCategory&) = delete;
		ErrorCodeCategory& operator=(ErrorCodeCategory&&) = delete;

		//
		// Overrides from std::error_category
		//
	public:
		/**
		 * @brief Category name
		 * @return A pointer to a null-terminated byte string that specifies the name of the error category.
		*/
		const char* name() const noexcept override;

		/**
		 * @brief Maps error_code to error_condition
		 * 
		 * Returns the default error_condition object of this category that is associated with
		 * the error_code identified by a value of code.
		 * 
		 * @param code A numerical value identifying an error
		 * @return The default error_condition associated with error value
		*/
		std::error_condition default_error_condition(int code) const noexcept override;
		
		/**
		 * @brief Compares error_code and error_condition for equivalence
		 * Checks whether, for the category, an error code is equivalent to an error condition, specifically
		 * Checks whether error code is equivalent to an error condition for the error category represented by *this
		 *
		 * @param code An object of an error_code type
		 * @param condition A numerical value identifying an error condition
		 * @return true if equivalent, false otherwise
		*/
		bool equivalent(const std::error_code& code, int condition) const noexcept override;
		
		/**
		 * @brief Compares error_code and error_condition for equivalence
		 * Checks whether, for the category, an error code is equivalent to an error condition, specifically
		 * Checks whether error code is equivalent to an error condition for the error category represented by *this
		 *
		 * @param code A numerical value identifying an error code
		 * @param condition An object of an error_condition type
		 * @return true if equivalent, false otherwise
		*/
		bool equivalent(int code, const std::error_condition& condition) const noexcept override;

		/**
		 * @brief Returns a string describing the given error
		 * 
		 * In derived classes, the function returns a string object with a message describing the error condition denoted by err_value.
		 * This function is called both by error_code::message and error_condition::message to obtain the corresponding message in the category.
		 * Therefore, numerical values used by custom error codes and error conditions should only match for a category if they describe the same error.
		 *
		 * @param code A numerical value identifying an error
		 * @return A string object with the message
		*/
		std::string message(int code) const override;
	} ErrorCategory;

	const char* ErrorCodeCategory::name() const noexcept
	{
		return "Error type";
	}

	std::error_condition ErrorCodeCategory::default_error_condition(int code) const noexcept
	{
		switch (static_cast<ErrorCode>(code))
		{
		case ErrorCode::Success:
			return make_error_condition(ErrorCondition::no_error);
		case ErrorCode::BadArgument:
		case ErrorCode::InvalidPointer:
		case ErrorCode::NotImplemented:
		case ErrorCode::OutOfRange:
		case ErrorCode::AlocationFailed:
		case ErrorCode::MemoryReadFailed:
		case ErrorCode::Unexpected:
		case ErrorCode::PointerNotNULL:
		case ErrorCode::CaseNotHandled:
		case ErrorCode::InvalidHandle:
		case ErrorCode::FunctionFailed:
		case ErrorCode::MemoryLeak:
		case ErrorCode::BadFunctionCall:
		case ErrorCode::ObjectNotValid:
		case ErrorCode::InitializationFailed:
		case ErrorCode::InvalidFlags:
		case ErrorCode::BadResult:
		case ErrorCode::RunTimeLibraryError:
		case ErrorCode::BadDelete:
		case ErrorCode::ExceptionInDestructor:
		case ErrorCode::UnhandledException:
		case ErrorCode::ResourceLeak:
		case ErrorCode::ParseFailure:
		case ErrorCode::UnsuportedOperation:
		case ErrorCode::BadVariableData:
		case ErrorCode::BadCast:
		case ErrorCode::NotFound:
			return make_error_condition(ErrorCondition::code_error);
		case ErrorCode::UnknownOption:
		case ErrorCode::InvalidParameter:
		case ErrorCode::InvalidCommand:
			return make_error_condition(ErrorCondition::user_error);
		case ErrorCode::UnspecifiedError:
			return make_error_condition(ErrorCondition::unspecified_error);
		default:
			// we cant show error here because function must be noexcept
			return make_error_condition(ErrorCondition::unspecified_error);
		}
	}

	bool ErrorCodeCategory::equivalent(const std::error_code& code, int condition) const noexcept
	{
		return (*this == code.category()) && (code.value() == condition);
	}

	bool ErrorCodeCategory::equivalent(int code, const std::error_condition& condition) const noexcept
	{
		return default_error_condition(code) == condition;
	}

	std::string ErrorCodeCategory::message(int code) const
	{
		switch (static_cast<ErrorCode>(code))
		{
		case ErrorCode::Success:
			return "All operations completed successfully";
			// Code error
		case ErrorCode::BadArgument:
			return "Bad Argument";
		case ErrorCode::InvalidPointer:
			return "Invalid pointer";
		case ErrorCode::NotImplemented:
			return "Not implemented";
		case ErrorCode::OutOfRange:
			return "Out of range";
		case ErrorCode::AlocationFailed:
			return "Memory allocation failed";
		case ErrorCode::MemoryReadFailed:
			return "Memory read failed";
		case ErrorCode::Unexpected:
			return "Unexpected execution flow";
		case ErrorCode::PointerNotNULL:
			return "Pointer is not NULL";
		case ErrorCode::CaseNotHandled:
			return "Case label not handled";
		case ErrorCode::InvalidHandle:
			return "Handle is NULL or invalid";
		case ErrorCode::FunctionFailed:
			return "Function call failed";
		case ErrorCode::MemoryLeak:
			return "Possible memory leak";
		case ErrorCode::BadFunctionCall:
			return "Bad function call";
		case ErrorCode::ObjectNotValid:
			return "Object is invalid";
		case ErrorCode::InitializationFailed:
			return "Initialization failed";
		case ErrorCode::InvalidFlags:
			return "Invalid bitwise flags";
		case ErrorCode::BadResult:
			return "Result is most likely wrong";
		case ErrorCode::RunTimeLibraryError:
			return "Runtime library error";
		case ErrorCode::BadDelete:
			return "Attempt to delete invalid memory";
		case ErrorCode::ExceptionInDestructor:
			return "Destructor threw an exception";
		case ErrorCode::UnhandledException:
			return "Unhanded exception, missing handler";
		case ErrorCode::ResourceLeak:
			return "Resource leak";
		case ErrorCode::ParseFailure:
			return "Parsing error";
		case ErrorCode::UnsuportedOperation:
			return "Unsupported operation";
		case ErrorCode::BadVariableData:
			return "Bad variable data";
		case ErrorCode::BadCast:
			return "Bad cast";
		case ErrorCode::NotFound:
			return "Item not found";
			// User error
		case ErrorCode::UnknownOption:
			return "Unknown option";
		case ErrorCode::InvalidParameter:
			return "Invalid option parameter";
		case ErrorCode::InvalidCommand:
			return "Invalid command";
			// Unspecified
		case ErrorCode::UnspecifiedError:
			return "Unspecified error";
		default:
			return "Unknown error";
		}
	}

	std::error_code make_error_code(ErrorCode code) noexcept
	{
		return std::error_code(
			// A numerical value identifying an error code
			static_cast<int>(code),

			// A reference to an error_category object
			ErrorCategory);
	}

	SUPPRESS(26497);	// function could be marked constexpr (yes but, can't be inlined)
	int ExitCode(ErrorCode err_enum) noexcept
	{
		return static_cast<int>(err_enum);
	}
}
