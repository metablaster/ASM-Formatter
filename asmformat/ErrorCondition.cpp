
/*
 *	Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file asmformat\ErrorCondition.cpp
 *
 * Definition of Error condition for error codes
 *
*/

#include "pch.hpp"
#include "ErrorCondition.hpp"
#include "error.hpp"
#include "ErrorCode.hpp"


/*
* Explanation of std error handling
*
* 1. std::system_error		(exception class/object being thrown) takes std::error_code (or individual parts of error_code) for construction
* 2. std::error_code		(object explaining the error) takes int error value and std::error_category or custom error enum for it's construction
*							in case of custom error enum 'make_error_code' must be overridden to construct error_code from error_category instance and error value
*
* 3. std::error_category	should be inherited and only a single instance of that class is used to describe error category,
*							see comment in declaration of error_category below for more info
* 4. std::error_condition	Stores an abstract error code and category.
*							Objects of this type describe errors in a generic way so that they may be portable across different systems.
*							This is in contrast with error_code objects, that may contain system-specific information.
*							error_condition objects are generally used to check whether a particular error_code obtained from the system matches a specific
*							error condition no matter the system.
*/

namespace wsl
{
	// Category types are used to identify the source of an error.
	// They also define the relation between error_code and error_condition objects of its category,
	// as well as the message set for error_code objects.
	class ErrorConditionCategory :
		public std::error_category
	{
		//
		// Constructors
		//
	public:
		ErrorConditionCategory() = default;
		~ErrorConditionCategory() = default;

	private:
		/*
		* Objects of these types have no distinct values and are not-copyable and not-assignable,
		* and thus can only be passed by reference.
		* As such, only one object of each of these types shall exist, each uniquely identifying its own category:
		* all error codes and conditions of a same category shall return a reference to same object.
		*/
		ErrorConditionCategory(const ErrorConditionCategory&) = delete;
		ErrorConditionCategory(ErrorConditionCategory&&) = delete;

		//
		// Operators
		//
		ErrorConditionCategory& operator=(const ErrorConditionCategory&) = delete;
		ErrorConditionCategory& operator=(ErrorConditionCategory&&) = delete;

	public:
		const char* name() const noexcept override;
		std::error_condition default_error_condition(int err_code_value) const noexcept override;
		bool equivalent(const std::error_code& err_code, int cond_err_value) const noexcept override;
		bool equivalent(int err_code_value, const std::error_condition& err_cond) const noexcept override;
		std::string message(int cond_err_value) const override;
	};

	// this is preferred method than global variable to avoid C26426
	inline static const ErrorConditionCategory& getDefaultCategory() noexcept
	{
		static const ErrorConditionCategory static_DefaultConditionCategory;
		return static_DefaultConditionCategory;
	}

	// Returns a pointer to a null-terminated byte string that specifies the name of the error category.
	const char* ErrorConditionCategory::name() const noexcept
	{
		// Since this is condition category (not error) this should never be called!
		return "WSL Default Condition";
	}

	// compares error_code and error_condition for equivalence
	// Checks whether, for the category, an error code is equivalent to an error condition, specifically
	// Checks whether error code is equivalent to an error condition for the error category represented by *this
	bool ErrorConditionCategory::equivalent(const std::error_code& err_code, int cond_err_value) const noexcept
	{
		// Workaround to get category for error code
		// TODO: can we avoid creating a temporary?
		const std::error_category& err_code_category_enum = std::error_code{ ErrorCode{} }.category();
		const std::error_category& err_code_category_param = err_code.category();

		switch (static_cast<ErrorCondition>(cond_err_value))
		{
		case ErrorCondition::no_error:
			if (err_code_category_param == err_code_category_enum)
				return err_code.value() == static_cast<int>(ErrorCode::Success);
			return false;
		case ErrorCondition::code_error:
			switch (static_cast<ErrorCode>(err_code.value()))
			{
			case ErrorCode::Success:
				break;
			case ErrorCode::BadArgument:
			case ErrorCode::InvalidPointer:
			case ErrorCode::NoImplementation:
			case ErrorCode::OutOfRange:
			case ErrorCode::AlocationFailed:
			case ErrorCode::MemoryReadFailed:
			case ErrorCode::CaseNotHandled:
			case ErrorCode::PointerNotNULL:
			case ErrorCode::Unexpected:
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
				return true;
			case ErrorCode::UnknownError:
				break;
			default:
				break;
			}
			[[fallthrough]];
		case ErrorCondition::unknown_error:
			if (err_code_category_param == err_code_category_enum)
				return err_code.value() == static_cast<int>(ErrorCode::UnknownError);
			return false;
		default:
			return false;
		}
	}

	// compares error_code and error_condition for equivalence
	// Checks whether, for the category, an error code is equivalent to an error condition, specifically
	// Checks whether error code is equivalent to an error condition for the error category represented by *this
	bool ErrorConditionCategory::equivalent(int err_code_value, const std::error_condition& err_cond) const noexcept
	{
		// Since this is condition category (not error) this should never be called!
		return default_error_condition(err_code_value) == err_cond;
	}

	std::error_condition make_error_condition(ErrorCondition cond_err_value) noexcept
	{
		return std::error_condition(static_cast<int>(cond_err_value), getDefaultCategory());
	}

	// maps error_code to error_condition.
	// Returns the error condition for the given error code.
	// Returns the default error_condition object of this category that is associated with the error_code identified by a value of err_value.
	std::error_condition ErrorConditionCategory::default_error_condition(int err_code_value) const noexcept
	{
		// Since this is condition category (not error) this should never be called!
		return std::error_condition(err_code_value, std::generic_category());
	}

	// Returns a string describing the given error condition for the error category represented by *this
	// In derived classes, the function returns a string object with a message describing the error condition denoted by err_value.
	// This function is called both by error_code::message and error_condition::message to obtain the corresponding message in the category.
	// Therefore, numerical values used by custom error codes and error conditions should only match for a category if they describe the same error.
	std::string ErrorConditionCategory::message(int cond_err_value) const
	{
		switch (static_cast<ErrorCondition>(cond_err_value))
		{
		case ErrorCondition::no_error:
			return "No error";
		case ErrorCondition::code_error:
			return "Code error";
		case ErrorCondition::unknown_error:
			return "Unknown error";
		default:
			ShowError(ErrorCode::Unexpected);
			return "UNHANDLED CASE!!";
		}
	}
}
