#ifndef __TINY_CORE__CONFIGURATION__OPTION__H__
#define __TINY_CORE__CONFIGURATION__OPTION__H__


/**
 *
 *  作者: hm
 *
 *  说明: 命令行参数
 *
 */


#include <tinyCore/debug/trace.h>
#include <tinyCore/utilities/time.h>


namespace tinyCore
{
	namespace configuration
	{
		class OptionDescription
		{
		public:
			OptionDescription() = default;

			OptionDescription(std::string option, std::string description, std::string value, bool isArg = false) :
					_isArg(isArg),
					_value(std::move(value)),
					_option(std::move(option)),
					_description(std::move(description))
			{

			}

			OptionDescription(OptionDescription && rhs) noexcept : _isArg(rhs._isArg),
																   _value(std::move(rhs._value)),
																   _option(std::move(rhs._option)),
																   _description(std::move(rhs._description))
			{

			}

			OptionDescription(const OptionDescription & rhs) = default;

			OptionDescription & operator=(OptionDescription && rhs) noexcept
			{
				_isArg = rhs._isArg;

				_value = std::move(rhs._value);
				_option = std::move(rhs._option);
				_description = std::move(rhs._description);

				return *this;
			}

			OptionDescription & operator=(const OptionDescription & rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				_isArg = rhs._isArg;

				_value = rhs._value;
				_option = rhs._option;
				_description = rhs._description;

				return *this;
			}

			bool IsArg() const
			{
				return _isArg;
			}

			const std::string & Value() const
			{
				return _value;
			}

			const std::string & Option() const
			{
				return _option;
			}

			const std::string & Description() const
			{
				return _description;
			}

		protected:
			bool _isArg{ false };

			std::string _value{ };
			std::string _option{ };
			std::string _description{ };
		};

		class OptionManager
		{
			using OptionDescriptionMap = std::map<std::string, std::shared_ptr<OptionDescription>>;

		public:
			OptionManager() = default;

			explicit OptionManager(std::string caption) : _caption(std::move(caption))
			{

			}

			OptionManager(OptionManager && rhs) noexcept : _caption(std::move(rhs._caption)),
														   _descriptions(std::move(rhs._descriptions))
			{

			}

			OptionManager(const OptionManager & rhs) = default;

			OptionManager & operator=(OptionManager && rhs) noexcept
			{
				_caption = std::move(rhs._caption);
				_descriptions = std::move(rhs._descriptions);

				return *this;
			}

			OptionManager & operator=(const OptionManager & rhs)
			{
				if (this == &rhs)
				{
					return *this;
				}

				_caption = rhs._caption;
				_descriptions = rhs._descriptions;

				return *this;
			}

			bool AddOption(const std::string & option, const std::string & description, const std::string & value = "", bool isArg = false)
			{
				if (HasOption(option))
				{
					return false;
				}

				_descriptions.insert(std::make_pair(option, std::make_shared<OptionDescription>(option, description, value, isArg)));

				return true;
			}

			const OptionDescriptionMap & Descriptions()
			{
				return _descriptions;
			}

		protected:
			template <typename TypeT>
			bool HasOption(const TypeT & option)
			{
				return _descriptions.find(option) != _descriptions.end();
			}

		protected:
			std::string _caption{ };

			OptionDescriptionMap _descriptions{ };
		};

		class OptionParse
		{
			using OptionManagerMap = std::map<std::string, std::shared_ptr<OptionManager>>;
			using OptionDescriptionMap = std::map<std::string, std::shared_ptr<OptionDescription>>;

		public:
			static OptionParse & Instance()
			{
				static OptionParse instance;

				return instance;
			}

			bool Has()
			{
				return !_parse.empty();
			}

			template <typename TypeT>
			bool Has(const TypeT & option)
			{
				return HasParse<TypeT>(option);
			}

			template <typename TypeT = std::string>
			const std::string & Get(const TypeT & option)
			{
				TINY_THROW_EXCEPTION_IF(!HasParse(option), debug::NotFound, TINY_STR_FORMAT("Can't Not Found Option ({})", option))

				return _parse[option]->Value();
			};

			void Define(const char * option, const char * description, const char * group = nullptr)
			{
				auto manager = GetGroup(group ? group : "Allowed options");

				if (manager->AddOption(option, description))
				{
					UpdateAlignment(option, nullptr);
				}
			}

			void DefineArg(const char * option, const char * description, const char * value = nullptr, const char * group = nullptr)
			{
				auto manager = GetGroup(group ? group : "Allowed options");

				if (manager->AddOption(option, description, value ? value : "", true))
				{
					UpdateAlignment(option, value);
				}
			}

			template <typename TypeT>
			void DefineVersion(const TypeT & version)
			{
				_version = version;
			}

			void Parse(const int32_t argc, char const * argv[])
			{
				Define("help", "display help message", "Help options");
				Define("version", "display version message", "Help options");

				ComposeOptions();

				std::string opt;
				std::string val;

				for (int32_t i = 1; i < argc; ++i)
				{
					TINY_THROW_EXCEPTION_IF(!TINY_STR_START_WITH(argv[i], "--"),
											debug::ParsingError,
											TINY_STR_FORMAT("Invalid Option [{}]", argv[i]))

					const char * find = strstr(argv[i] + 2, "=");

					if (find)
					{
						val.assign(find + 1);
						opt.assign(argv[i] + 2, find);
					}
					else
					{
						val.assign("");
						opt.assign(argv[i] + 2);
					}

					TINY_THROW_EXCEPTION_IF(HasParse(opt),
											debug::ParsingError,
											TINY_STR_FORMAT("Repeat Option [{}]", argv[i]))

					TINY_THROW_EXCEPTION_IF(!HasOption(opt),
											debug::ParsingError,
											TINY_STR_FORMAT("Unrecognized Option [{}]", argv[i]))

					TINY_THROW_EXCEPTION_IF(_options[opt]->IsArg() && find == nullptr,
											debug::ParsingError,
											TINY_STR_FORMAT("Arg Option Not Input Value [{}]", argv[i]))

					_parse.insert(std::make_pair(opt, std::make_shared<OptionDescription>(opt, _options[opt]->Description(), val)));
				}

				CheckDefaultOption();

				if (HasParse("help"))
				{
					HelpCallBack();
				}

				if (HasParse("version"))
				{
					VersionCallBack();
				}
			}

		protected:
			template <typename TypeT>
			bool HasParse(const TypeT & option)
			{
				return _parse.find(option) != _parse.end();
			}

			template <typename TypeT>
			bool HasOption(const TypeT & option)
			{
				return _options.find(option) != _options.end();
			}

			void UpdateAlignment(const char * option, const char * value)
			{
				if (value)
				{
					UpdateValueSize(strlen(value));
				}

				if (option)
				{
					UpdateOptionSize(strlen(option));
				}
			}

			void UpdateValueSize(const std::size_t size)
			{
				if (_valSize < size)
				{
					_valSize = size;
				}
			}

			void UpdateOptionSize(const std::size_t size)
			{
				if (_optSize < size)
				{
					_optSize = size;
				}
			}

			void ComposeOptions()
			{
				for (auto &group : _groups)
				{
					if (group.second)
					{
						for (auto &iter : group.second->Descriptions())
						{
							_options.insert(std::make_pair(iter.first, iter.second));
						}
					}
				}
			}

			void CheckDefaultOption()
			{
				for (auto &iter : _options)
				{
					if (iter.second)
					{
						if (iter.second->Value().empty() || HasParse(iter.first))
						{
							continue;
						}

						_parse.insert(std::make_pair(iter.first, iter.second));
					}
				}
			}

			std::shared_ptr<OptionManager> GetGroup(const char * group)
			{
				TINY_THROW_EXCEPTION_IF(group == nullptr, debug::Nullptr, "nullptr")

				auto iter = _groups.find(group);

				if (iter == _groups.end())
				{
					auto options = std::make_shared<OptionManager>(group);

					_groups.insert(std::make_pair(group, options));

					return options;
				}
				else
				{
					return iter->second;
				}
			}

			void DisplayDescription(const OptionDescriptionMap & descriptions)
			{
				for (auto &iter : descriptions)
				{
					std::cout << std::setw(static_cast<int32_t>(_optSize)) << std::right << iter.first;
					std::cout << "    ";

					if (iter.second->IsArg())
					{
						std::cout << "arg";

						if (iter.second->Value().empty())
						{
							std::cout << "  ";
						}
						else
						{
							std::cout << "(" << iter.second->Value() << ")";
						}

						auto size = static_cast<int32_t>(_valSize - iter.second->Value().size());

						if (size > 0)
						{
							std::cout << std::setw(size) << std::right << " ";
						}
					}
					else
					{
						std::cout << std::setw(static_cast<int32_t>(_valSize + 5)) << std::right << " ";
					}

					std::cout << "    " << iter.second->Description() << std::endl;
				}
			}

			void HelpCallBack()
			{
				for (auto &group : _groups)
				{
					if (group.second)
					{
						std::cout << std::endl << group.first << ":" << std::endl;

						DisplayDescription(group.second->Descriptions());
					}
				}

				std::cout << std::endl;

				exit(0);
			}

			void VersionCallBack()
			{
				std::cout << _version << std::endl;

				exit(0);
			}

		protected:
			std::size_t _optSize{ 0 };
			std::size_t _valSize{ 0 };

			std::string _version{ TINY_CORE_VERSION_STRING };

			OptionManagerMap _groups{ };

			OptionDescriptionMap _parse{ };
			OptionDescriptionMap _options{ };
		};
	}
}


#define TINY_OPTION_HAS(...)								tinyCore::configuration::OptionParse::Instance().Has(__VA_ARGS__)
#define TINY_OPTION_GET(option)								tinyCore::configuration::OptionParse::Instance().Get(option)

#define TINY_OPTION_PARSE(argc, argv)						tinyCore::configuration::OptionParse::Instance().Parse(argc, argv);
#define TINY_OPTION_DEFINE(option, description, ...)		tinyCore::configuration::OptionParse::Instance().Define(option, description, ##__VA_ARGS__);
#define TINY_OPTION_DEFINE_ARG(option, description, ...)	tinyCore::configuration::OptionParse::Instance().DefineArg(option, description, ##__VA_ARGS__);
#define TINY_OPTION_DEFINE_VERSION(version)					tinyCore::configuration::OptionParse::Instance().DefineVersion(version);


#endif // __TINY_CORE__CONFIGURATION__OPTION__H__
