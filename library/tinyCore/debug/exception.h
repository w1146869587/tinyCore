#ifndef __TINY_CORE__DEBUG__EXCEPTION__H__
#define __TINY_CORE__DEBUG__EXCEPTION__H__


/**
 *
 *  作者: hm
 *
 *  说明: 异常管理
 *
 */


#include <exception>
#include <stdexcept>

#include <tinyCore/common/common.h>


namespace tinyCore
{
	namespace debug
	{
		#define DECLARE_EXCEPTION(Exception, ParentException)		\
																	\
		class TINY_API Exception: public ParentException			\
		{															\
		public:														\
			using ParentException::ParentException;					\
																	\
			Exception()  = default;									\
			~Exception() = default;									\
		}

		class TINY_API SystemExit
		{
		public:
			SystemExit(const char * file, const int32_t line, const char * func,
					   std::string message = "", std::string option = "")
					: _line(line),
					  _file(file),
					  _func(func),
					  _option(std::move(option)),
					  _message(std::move(message))
			{

			}

			const char * what() const
			{
				if (_what.empty())
				{
					if (!_message.empty())
					{
						_what += "  ";
						_what += "what():  ";
						_what += _message;
						_what += "\n";
					}

					_what += "  ";
					_what += "find():  ";
					_what += _file;
					_what += ':';
					_what += std::to_string(_line);
					_what += " in ";
					_what += _func;

					if (!_option.empty())
					{
						_what += "\n\n";
						_what += _option;
					}
				}

				return _what.data();
			}

		protected:
			int32_t _line;

			const char * _file;
			const char * _func;

			mutable std::string _what;
			mutable std::string _option;
			mutable std::string _message;
		};

		class TINY_API IException : public std::runtime_error
		{
		public:
			IException(const char * file, const int32_t line, const char * func,
					   const std::string & message = "", std::string option = "")
					: std::runtime_error(message),
					  _line(line),
					  _file(file),
					  _func(func),
					  _option(std::move(option))
			{

			}

			const char * what() const noexcept override
			{
				if (_what.empty())
				{
					_what += std::runtime_error::what();
					_what += "\n  ";
					_what += "find():  ";
					_what += _file;
					_what += ':';
					_what += std::to_string(_line);
					_what += " in ";
					_what += _func;

					if (!_option.empty())
					{
						_what += "\n\n";
						_what += _option;
					}
				}

				return _what.data();
			}

		protected:
			int32_t _line;

			const char * _file;
			const char * _func;

			mutable std::string _what;
			mutable std::string _option;
		};

		class TINY_API ICodeException : public std::runtime_error
		{
		public:
			ICodeException(const char * file, const int32_t line, const char * func, const int32_t code,
						   const std::string & message = "", std::string option = "")
					: std::runtime_error(message),
					  _line(line),
					  _code(code),
					  _file(file),
					  _func(func),
					  _option(std::move(option))
			{

			}

			const char * what() const noexcept override
			{
				if (_what.empty())
				{
					_what += std::runtime_error::what();
					_what += "\n  ";
					_what += "code():  ";
					_what += std::to_string(_code);
					_what += "\n  ";
					_what += "find():  ";
					_what += _file;
					_what += ':';
					_what += std::to_string(_line);
					_what += " in ";
					_what += _func;

					if (!_option.empty())
					{
						_what += "\n\n";
						_what += _option;
					}
				}

				return _what.data();
			}

		protected:
			int32_t _line;
			int32_t _code;

			const char * _file;
			const char * _func;

			mutable std::string _what;
			mutable std::string _option;
		};

		class TINY_API AssertionError : public IException
		{
		public:
			AssertionError(const char * file, const int32_t line, const char * func, const char * cond,
						   const std::string & message = "", std::string option = "")
				: IException(file, line, func, message, std::move(option)),
				  _cond(cond)
			{

			}

			const char * what() const noexcept override
			{
				if (_what.empty())
				{
					_what += std::runtime_error::what();
					_what += "\n  ";
					_what += "cond():  ";
					_what += _cond;
					_what += "\n  ";
					_what += "find():  ";
					_what += _file;
					_what += ':';
					_what += std::to_string(_line);
					_what += " in ";
					_what += _func;

					if (!_option.empty())
					{
						_what += "\n\n";
						_what += _option;
					}
				}

				return _what.c_str();
			}

		protected:
			std::string _cond;
		};

		DECLARE_EXCEPTION(Exception, IException);
		DECLARE_EXCEPTION(Nullptr, Exception);
		DECLARE_EXCEPTION(NotFound, Exception);
		DECLARE_EXCEPTION(KeyError, Exception);
		DECLARE_EXCEPTION(EOFError, Exception);
		DECLARE_EXCEPTION(SizeError, Exception);
		DECLARE_EXCEPTION(ModeError, Exception);
		DECLARE_EXCEPTION(ForkError, Exception);
		DECLARE_EXCEPTION(CurlError, Exception);
		DECLARE_EXCEPTION(FileError, Exception);
		DECLARE_EXCEPTION(ZlibError, Exception);
		DECLARE_EXCEPTION(TypeError, Exception);
		DECLARE_EXCEPTION(ValueError, Exception);
		DECLARE_EXCEPTION(IndexError, Exception);
		DECLARE_EXCEPTION(ThreadError, Exception);
		DECLARE_EXCEPTION(MemoryError, Exception);
		DECLARE_EXCEPTION(ParsingError, Exception);
		DECLARE_EXCEPTION(TimeoutError, Exception);
		DECLARE_EXCEPTION(DuplicateKey, Exception);
		DECLARE_EXCEPTION(PermissionError, Exception);
		DECLARE_EXCEPTION(IllegalArguments, Exception);
		DECLARE_EXCEPTION(EnvironmentError, Exception);
		DECLARE_EXCEPTION(ZeroDivisionError, Exception);
		DECLARE_EXCEPTION(NotImplementedError, Exception);
		DECLARE_EXCEPTION(IOError, EnvironmentError);

		DECLARE_EXCEPTION(CodeException, ICodeException);
		DECLARE_EXCEPTION(HTTPError, CodeException);
		DECLARE_EXCEPTION(MysqlError, CodeException);
		DECLARE_EXCEPTION(SQLiteError, CodeException);

		class TINY_API ExceptionHelper
		{
		public:
			template <typename Error, typename... Args>
			static void ThrowException(const char * file, const int32_t line, const char * func, Args &&... args)
			{
				throw Error(file, line, func, std::forward<Args>(args)...);
			}

			template <typename Error, typename... Args>
			static Error MakeException(const char * file, const int32_t line, const char * func, Args &&... args)
			{
				return Error(file, line, func, std::forward<Args>(args)...);
			}

			template <typename Error, typename... Args>
			static std::exception_ptr MakeExceptionPtr(const char * file, const int32_t line, const char * func, Args &&... args)
			{
				return std::make_exception_ptr(Error(file, line, func, std::forward<Args>(args)...));
			}
		};
	}
}


#define TINY_MAKE_EXCEPTION(Exception, ...)		tinyCore::debug::ExceptionHelper::MakeException<Exception>(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);
#define TINY_THROW_EXCEPTION(Exception, ...)	tinyCore::debug::ExceptionHelper::ThrowException<Exception>(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);
#define TINY_MAKE_EXCEPTION_PTR(Exception, ...)	tinyCore::debug::ExceptionHelper::MakeExceptionPtr<Exception>(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);

#define TINY_THROW_EXCEPTION_IF(cond, Exception, ...)	if ( (cond)) tinyCore::debug::ExceptionHelper::ThrowException<Exception>(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__);


#endif // __TINY_CORE__DEBUG__EXCEPTION__H__
