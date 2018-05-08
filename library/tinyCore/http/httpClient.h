#ifndef __TINY_CORE__HTTP__HTTP_CLIENT__H__
#define __TINY_CORE__HTTP__HTTP_CLIENT__H__


/**
 *
 *  作者: hm
 *
 *  说明: curl实现http客户端
 *
 */


#include <curl/curl.h>

#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace http
	{
		enum class TINY_HTTP_ACTION : uint8_t
		{
			GET,
			POST,
		};

		class HTTPClient
		{
		public:
			explicit HTTPClient(TINY_HTTP_ACTION action = TINY_HTTP_ACTION::POST) : _action(action)
			{

			}

			~HTTPClient()
			{
				Clear();
			}

			bool Get()
			{
				SetAction(TINY_HTTP_ACTION::GET);

				return Launch();
			}

			bool Post()
			{
				SetAction(TINY_HTTP_ACTION::POST);

				return Launch();
			}

			bool Launch()
			{
				ClearResponse();

				CURL * handle = curl_easy_init();

				TINY_THROW_EXCEPTION_IF(handle == nullptr, debug::CurlError, "Curl init failed")

				if (_action == TINY_HTTP_ACTION::POST)
				{
					curl_easy_setopt(handle, CURLOPT_POST, 1);

					if (!_params.empty())
					{
						curl_easy_setopt(handle, CURLOPT_POSTFIELDS, _params.c_str());
					}
				}

				if (_port)
				{
					curl_easy_setopt(handle, CURLOPT_PORT, _port);
				}

				if (!_login.empty())
				{
					curl_easy_setopt(handle, CURLOPT_USERPWD, _login.c_str());
				}

				if (_headers)
				{
					curl_easy_setopt(handle, CURLOPT_HTTPHEADER, _headers);
				}

				curl_easy_setopt(handle, CURLOPT_VERBOSE, 0L);
				curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLSSH_AUTH_ANY);
				curl_easy_setopt(handle, CURLOPT_URL, _url.c_str());

				curl_easy_setopt(handle, CURLOPT_WRITEDATA, &_response);
				curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, RespondFunction);

#if 0
				curl_easy_setopt(handle, CURLOPT_READDATA, NULL);
				curl_easy_setopt(handle, CURLOPT_READFUNCTION, NULL);

				curl_easy_setopt(handle, CURLOPT_HEADERDATA, NULL);
				curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, NULL);
#endif

				// https不验证证书
				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
				// https不验证HOST
				curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);

				// 当多个线程都使用超时处理的时候, 同时主线程中有sleep或是wait等操作. 如果不设置这个选项, libcurl将会发信号打断wait从而导致程序退出.
				curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
				// 查找次数，防止查找太深
				curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 1);
				// 返回的头部中有Location(一般直接请求的url没找到), 则继续请求Location对应的数据
				curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);

				//接收数据时超时设置
				curl_easy_setopt(handle, CURLOPT_TIMEOUT, 2);
				// 连接超时
				curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, 2);

				_code = curl_easy_perform(handle);

				curl_easy_cleanup(handle);

				return _code == CURLE_OK;
			}

			const uint32_t Code() const
			{
				return _code;
			}

			const uint16_t Port() const
			{
				return _port;
			}

			const char * LastError() const
			{
				return curl_easy_strerror(_code);
			}

			const std::string & Url() const
			{
				return _url;
			}

			const std::string & User() const
			{
				return _user;
			}

			const std::string & Pass() const
			{
				return _pass;
			}

			const std::string & Login() const
			{
				return _login;
			}

			const std::string & Params() const
			{
				return _params;
			}

			const std::string & Response() const
			{
				return _response;
			}

			void Clear()
			{
				ClearCode();
				ClearPort();
				ClearLogin();
				ClearParams();
				ClearHeader();
				ClearResponse();
			}

			void ClearCode()
			{
				_code = CURLE_OK;
			}

			void ClearPort()
			{
				_port = 0;
			}

			void ClearLogin()
			{
				_user.clear();
				_pass.clear();
				_login.clear();
			}

			void ClearParams()
			{
				_params.clear();
			}

			void ClearHeader()
			{
				if (_headers)
				{
					curl_slist_free_all(_headers);

					_headers = nullptr;
				}
			}

			void ClearResponse()
			{
				_response.clear();
			}

			void SetPort(const uint16_t port)
			{
				_port = port;
			}

			void SetAction(TINY_HTTP_ACTION action)
			{
				_action = action;
			}

			void SetUrl(std::string url)
			{
				_url = std::move(url);
			}

			template <typename TypeT>
			void SetUrl(const TypeT & url)
			{
				_url = url;
			}

			void SetLogin(std::string user, std::string pass)
			{
				_login = TINY_STR_FORMAT("{}:{}", _user = std::move(user), _pass = std::move(pass));
			}

			template <typename TypeT>
			void SetLogin(const TypeT & user, const TypeT & pass)
			{
				_login = TINY_STR_FORMAT("{}:{}", _user = user, _pass = pass);
			}

			void SetHeader(const char * header)
			{
				_headers = curl_slist_append(_headers, header);
			}

			void SetHeader(std::string header)
			{
				_headers = curl_slist_append(_headers, std::move(header).c_str());
			}

			void SetHeader(const std::string & header)
			{
				_headers = curl_slist_append(_headers, header.c_str());
			}

			void SetParam(std::string param)
			{
				_params = std::move(param);
			}

			template <typename TypeT>
			void SetParam(const TypeT & param)
			{
				_params = param;
			}

			void SetParam(std::string key, std::string value)
			{
				if (!_params.empty())
				{
					_params += "&";
				}

				_params += TINY_STR_FORMAT("{}={}", std::move(key), std::move(value));
			}

			template <typename TypeT>
			void SetParam(const TypeT & key, const TypeT & value)
			{
				if (!_params.empty())
				{
					_params += "&";
				}

				_params += TINY_STR_FORMAT("{}={}", key, value);
			}

		protected:
			static size_t RespondFunction(void * buffer, size_t size, size_t nmemb, void * stream)
			{
				auto str = dynamic_cast<std::string *>((std::string *)stream);

				if (str == nullptr)
				{
					TINY_THROW_EXCEPTION(tinyCore::debug::CurlError, "Curl respondFunction stream null point");

					return 0;
				}

				std::size_t realSize = (std::size_t)(nmemb * size);

				str->append((char *)buffer, realSize);

				return realSize;
			}

		protected:
			CURLcode _code{ CURLE_OK };

			uint16_t _port{ 0 };

			std::string _url{ };
			std::string _user{ };
			std::string _pass{ };
			std::string _login{ };
			std::string _params{ };
			std::string _response{ };

			TINY_HTTP_ACTION _action{ TINY_HTTP_ACTION::POST };

			struct curl_slist * _headers{ nullptr };
		};
	}
}


#define TINY_HTTP_ACTION_GET tinyCore::http::TINY_HTTP_ACTION::GET
#define TINY_HTTP_ACTION_POST tinyCore::http::TINY_HTTP_ACTION::POST


#endif // __TINY_CORE__HTTP__HTTP_CLIENT__H__
