
/*
 * Project: "ASM Formatter" https://github.com/metablaster/ASM-Formatter
 * Copyright(C) 2023 metablaster (zebal@protonmail.ch)
 * Licensed under the MIT license
 *
*/

/**
 * @file asmformat\ErrorCondition.cpp
 *
 * Definition of custom error_condition
 *
*/

#include "pch.hpp"
#include "ErrorCondition.hpp"
#include "ErrorCode.hpp"


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
		
		// TODO: Needs testing
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
		 * @param condition A numerical value identifying an error condition
		 * @return A string object with the message
		*/
		std::string message(int condition) const override;
	} ConditionCategory;

	const char* ErrorConditionCategory::name() const noexcept
	{
		return "Error source";
	}

	std::error_condition ErrorConditionCategory::default_error_condition(int code) const noexcept
	{
		// Since this is condition category (not error) this should never be called!
		return std::error_condition(code, std::generic_category());
	}

	bool ErrorConditionCategory::equivalent(const std::error_code& code, int condition) const noexcept
	{
		// Workaround to get category for error code
		// TODO: can we avoid creating a temporary?
		const std::error_category& code_category_enum = std::error_code{ ErrorCode{} }.category();
		const std::error_category& code_category_param = code.category();

		switch (static_cast<ErrorCondition>(condition))
		{
		case ErrorCondition::no_error:
			if (code_category_param == code_category_enum)
				return code.value() == static_cast<int>(ErrorCode::Success);
			return false;

		case ErrorCondition::code_error:
			switch (static_cast<ErrorCode>(code.value()))
			{
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
				return true;
			default:
				return false;
			}

		case ErrorCondition::user_error:
			switch (static_cast<ErrorCode>(code.value()))
			{
			case ErrorCode::UnknownOption:
			case ErrorCode::InvalidParameter:
			case ErrorCode::InvalidCommand:
				return true;
			default:
				return false;
			}

		case ErrorCondition::unspecified_error:
			if (code_category_param == code_category_enum)
				return code.value() == static_cast<int>(ErrorCode::UnspecifiedError);
			return false;

		default:
			return false;
		}
	}

	bool ErrorConditionCategory::equivalent(int code, const std::error_condition& condition) const noexcept
	{
		// Since this is condition category (not error) this should never be called!
		return default_error_condition(code) == condition;
	}

	std::string ErrorConditionCategory::message(int condition) const
	{
		switch (static_cast<ErrorCondition>(condition))
		{
		case ErrorCondition::no_error:
			return "No error";
		case ErrorCondition::code_error:
			return "Code error";
		case ErrorCondition::user_error:
			return "User error";
		case ErrorCondition::unspecified_error:
			return "Unspecified error";
		default:
			return "Unknown error";
		}
	}

	std::error_condition make_error_condition(ErrorCondition condition) noexcept
	{
		return std::error_condition(
			// A numerical value identifying an error condition
			static_cast<int>(condition),
			
			// A reference to an error_category object
			ConditionCategory);
	}
}
