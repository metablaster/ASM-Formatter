
/*
 *	Project: "masm-formatter" https://github.com/metablaster/ASM-Formatter
 *	Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 *	Licensed under the MIT license
 *
*/

/**
 * @file mlfmt\ErrorCode.cpp
 *
 * Definition of code error code
 *
*/

#include "pch.hpp"
#include "ErrorCode.hpp"
#include "error.hpp"
#include "ErrorCondition.hpp"


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
		const char* name() const noexcept override;
		std::error_condition default_error_condition(int err_code_value) const noexcept override;
		bool equivalent(const std::error_code& err_code, int cond_err_value) const noexcept override;
		bool equivalent(int err_code_value, const std::error_condition& err_cond) const noexcept override;
		std::string message(int err_value) const override;
	};

	// this is preferred method than global variable to avoid C26426
	inline static const ErrorCodeCategory& getGenericErrorCategory() noexcept
	{
		static const ErrorCodeCategory static_GenericErrorCategory;
		return static_GenericErrorCategory;
	}

	std::error_code make_error_code(ErrorCode err_code_value) noexcept
	{
		return std::error_code(static_cast<int>(err_code_value), getGenericErrorCategory());
	}

	// Returns a pointer to a null-terminated byte string that specifies the name of the error category.
	const char* ErrorCodeCategory::name() const noexcept
	{
		return "WSL Error";
	}

	// compares error_code and error_condition for equivalence
	// Checks whether, for the category, an error code is equivalent to an error condition, specifically
	// Checks whether error code is equivalent to an error condition for the error category represented by *this
	bool ErrorCodeCategory::equivalent(const std::error_code& err_code, int cond_err_value) const noexcept
	{
		return *this == err_code.category() &&
			static_cast<int>(default_error_condition(err_code.value()).value()) == cond_err_value;
	}

	// compares error_code and error_condition for equivalence
	// Checks whether, for the category, an error code is equivalent to an error condition, specifically
	// Checks whether error code is equivalent to an error condition for the error category represented by *this
	bool ErrorCodeCategory::equivalent(int err_code_value, const std::error_condition& err_cond) const noexcept
	{
		return default_error_condition(err_code_value) == err_cond;
	}

	// maps error_code to error_condition.
	// Returns the error condition for the given error code.
	// Returns the default error_condition object of this category that is associated with the error_code identified by a value of err_value.
	std::error_condition ErrorCodeCategory::default_error_condition(int err_code_value) const noexcept
	{
		switch (static_cast<ErrorCode>(err_code_value))
		{
		case ErrorCode::Success:
			return make_error_condition(ErrorCondition::no_error);
		case ErrorCode::BadArgument:
		case ErrorCode::InvalidPointer:
		case ErrorCode::NoImplementation:
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
		case ErrorCode::UnknownError:
			return make_error_condition(ErrorCondition::unknown_error);
		default:
			// we cant show error here because function must be noexcept
			return make_error_condition(ErrorCondition::unknown_error);
		}
	}

	// Returns a string describing the given error condition for the error category represented by *this
	// In derived classes, the function returns a string object with a message describing the error condition denoted by err_value.
	// This function is called both by error_code::message and error_condition::message to obtain the corresponding message in the category.
	// Therefore, numerical values used by custom error codes and error conditions should only match for a category if they describe the same error.
	std::string ErrorCodeCategory::message(int err_value) const
	{
		switch (static_cast<ErrorCode>(err_value))
		{
		case ErrorCode::Success:
			return "All operations completed successfully";
		case ErrorCode::BadArgument:
			return "Bad Argument";
		case ErrorCode::InvalidPointer:
			return "Invalid pointer";
		case ErrorCode::NoImplementation:
			return "No implementation";
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
		case ErrorCode::UnknownError:
			return "Unknown error";
		case ErrorCode::BadCast:
			return "Bad cast";
		case ErrorCode::NotFound:
			return "Item not found";
		default:
			ShowError(ErrorCode::Unexpected);
			return "UNHANDLED CASE!!";
		}
	}

	SUPPRESS(26497);	// function could be marked constexpr (yes but, can't be inlined)
	int ExitCode(ErrorCode err_enum) noexcept
	{
		return static_cast<int>(err_enum);
	}
}
