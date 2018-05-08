#ifndef __TINY_CORE__SYSTEM__SERIAL__H__
#define __TINY_CORE__SYSTEM__SERIAL__H__


/**
 *
 *  作者: hm
 *
 *  说明: 序列号
 *
 */


#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace system
	{
		class Serial
		{
		public:
			static bool CPUSerial(std::string & serial)
			{
				serial.clear();

				if (CPUSerialFromASM(serial))
				{
					return true;
				}

				if (getuid())
				{
					TINY_WARNING(strerror(errno));

					return false;
				}
				else
				{
					return CPUSerialFromCommand(serial);
				}
			}

			static bool DiskSerial(std::string & serial)
			{
				serial.clear();

				if (getuid())
				{
					TINY_WARNING(strerror(errno));

					return false;
				}

				std::string diskName;

				if (DiskName(diskName))
				{
					if (DiskSerialFirstMethod(diskName, serial))
					{
						return true;
					}
					else if (DiskSerialSecondMethod(diskName, serial))
					{
						return true;
					}
					else if (DiskSerialThirdMethod(diskName, serial))
					{
						return true;
					}
				}

				return DiskSerialFourthMethod(serial);
			}

			static bool BoardSerial(std::string & serial)
			{
				serial.clear();

				if (getuid())
				{
					TINY_WARNING(strerror(errno));

					return false;
				}

				return BoardSerialFromCommand(serial);
			}

		protected:
			static std::string CPUVendor()
			{
				std::string serial;

				uint32_t eax = 0;
				uint32_t ebx = 0;
				uint32_t ecx = 0;
				uint32_t edx = 0;

				asm volatile
				(
					"cpuid": "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)

					: "0"(0)
				);

				serial += TINY_HEX_STR_TO_STRING(TINY_DIGITAL_TO_HEX_STR(htonl(ebx)));
				serial += TINY_HEX_STR_TO_STRING(TINY_DIGITAL_TO_HEX_STR(htonl(edx)));
				serial += TINY_HEX_STR_TO_STRING(TINY_DIGITAL_TO_HEX_STR(htonl(ecx)));

				return serial;
			}

			static bool ScsiIO(const int32_t fd, const int32_t dxfer_dir, uint8_t * cdb, const uint8_t cdb_len,
																		  uint8_t * sense, const uint8_t sense_len,
																		  uint8_t * data, const uint32_t data_len)
			{
				TINY_ASSERT(cdb, "cdb is nullptr")
				TINY_ASSERT(data, "data is nullptr")
				TINY_ASSERT(sense, "sense is nullptr")

				sg_io_hdr_t io_hdr = { };

				io_hdr.interface_id = 'S';

				/* CDB */
				io_hdr.cmdp = cdb;
				io_hdr.cmd_len = cdb_len;

				/* Where to store the sense_data, if there was an error */
				io_hdr.sbp = sense;
				io_hdr.mx_sb_len = sense_len;

				/* Transfer direction, either in or out. Linux does not yet support bidirectional SCSI transfers ? */
				io_hdr.dxfer_direction = dxfer_dir;

				/* Where to store the DATA IN/OUT from the device and how big the buffer is */
				io_hdr.dxferp = data;
				io_hdr.dxfer_len = data_len;

				/* SCSI timeout in ms */
				io_hdr.timeout = 5000;

				if (ioctl(fd, SG_IO, &io_hdr) < 0)
				{
					TINY_WARNING(strerror(errno));

					return false;
				}

				/* now for the error processing */
				if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK && io_hdr.sb_len_wr > 0)
				{
					return false;
				}

				return !(io_hdr.masked_status || io_hdr.host_status || io_hdr.driver_status);
			}

			static bool DiskName(std::string & name)
			{
				std::ifstream ifs("/etc/mtab", std::ios::binary);

				if (!ifs.is_open())
				{
					return false;
				}

				char line[4096] = { 0 };

				while (!ifs.eof())
				{
					ifs.getline(line, sizeof(line));

					if (!ifs.good())
					{
						break;
					}

					const char * disk = line;

					while(isspace(disk[0]))
					{
						++disk;
					}

					const char * space = strchr(disk, ' ');

					if (space == nullptr)
					{
						continue;
					}

					const char * mount = space + 1;

					while(isspace(mount[0]))
					{
						++mount;
					}

					if (mount[0] != '/' || mount[1] != ' ')
					{
						continue;
					}

					while(space > disk && isdigit(space[-1]))
					{
						--space;
					}

					if (space > disk)
					{
						std::string(disk, space).swap(name);

						break;
					}
				}

				ifs.close();

				return !name.empty();
			}

			static bool CPUSerialFromASM(std::string & serial)
			{
				uint32_t eax = 0;
				uint32_t ebx = 0;
				uint32_t ecx = 0;
				uint32_t edx = 0;

				asm volatile
				(
					"cpuid": "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)

					: "0"(0)
				);

				serial += TINY_HEX_STR_TO_STRING(TINY_DIGITAL_TO_HEX_STR(htonl(ebx)));
				serial += TINY_HEX_STR_TO_STRING(TINY_DIGITAL_TO_HEX_STR(htonl(edx)));
				serial += TINY_HEX_STR_TO_STRING(TINY_DIGITAL_TO_HEX_STR(htonl(ecx)));

				uint32_t s1 = 0;
				uint32_t s2 = 0;

				asm volatile
				(
					"movl $0x01 , %%eax ; \n\t"

					"xorl %%edx , %%edx ;\n\t"

					"cpuid ;\n\t"

					"movl %%edx ,%0 ;\n\t"

					"movl %%eax ,%1 ; \n\t"

					:"=m"(s1),"=m"(s2)
				);

				if (s1 == 0 && s2 == 0)
				{
					return false;
				}

				serial += "-";
				serial += TINY_DIGITAL_TO_HEX_STR(htonl(s2), "%08x");
				serial += TINY_DIGITAL_TO_HEX_STR(htonl(s1), "%08x");

				return true;
			}

			static bool CPUSerialFromCommand(std::string & serial)
			{
				const char * file = ".dmidecode_result.txt";

				std::string op = TINY_STR_TO_STRING("dmidecode -t 4 2>/dev/null | grep ID > ", file);

				if (::system(op.c_str()) == 0)
				{
					CPUSerialFromFile(file, "ID:", serial);
				}

				unlink(file);  // 删除文件的目录项并减少它的链接数

				if (serial.empty())
				{
					return false;
				}
				else
				{
					serial.insert(0, TINY_STR_TO_STRING(Serial::CPUVendor(), "-"));

					return true;
				}
			}

			static bool BoardSerialFromCommand(std::string & serial)
			{
				const char * file = ".dmidecode_result.txt";

				std::string op = TINY_STR_TO_STRING("dmidecode -t 2 2>/dev/null | grep Serial > ", file);

				if (::system(op.c_str()) == 0)
				{
					BoardSerialFromFile(file, "Serial Number:", serial);
				}

				unlink(file);  // 删除文件的目录项并减少它的链接数

				return(!serial.empty());
			}

			static bool DiskSerialFirstMethod(const std::string & diskName, std::string & serial)
			{
				int32_t fd = open(diskName.c_str(), O_RDONLY);

				if (fd == -1)
				{
					TINY_WARNING(strerror(errno));

					return false;
				}

				struct hd_driveid drive = { };

				/*
				 * int ioctl(int fd, ind cmd, …)
				 *
				 * fd 是用户程序打开设备时使用open函数返回的文件标示符
				 *
				 * cmd 是用户程序对设备的控制命令
				 *
				 * 后面的省略号, 那是一些补充参数, 一般最多一个, 这个参数的有无和cmd的意义相关
				 *
				 */
				if (ioctl(fd, HDIO_GET_IDENTITY, &drive) == 0)
				{
					TrimSerial(drive.serial_no, sizeof(drive.serial_no), serial);
				}

				close(fd);

				return (!serial.empty());
			}

			static bool DiskSerialSecondMethod(const std::string & diskName, std::string & serial)
			{
				int32_t fd = open(diskName.c_str(), O_RDONLY);

				if (fd == -1)
				{
					TINY_WARNING(strerror(errno));

					return false;
				}

				int version = 0;

				if (ioctl(fd, SG_GET_VERSION_NUM, &version) < 0 || version < 30000)
				{
					TINY_WARNING(strerror(errno));

					close(fd);

					return false;
				}

				const uint8_t  senseLen = 32;
				const uint32_t dataLen  = 0x00ff;

				uint8_t data[dataLen]   = { 0 };
				uint8_t sense[senseLen] = { 0 };

				uint8_t cdb[] =
				{
					0x12, 0x01, 0x80, 0x00, 0x00, 0x00
				};

				cdb[3] = (dataLen >> 8) & 0xff;
				cdb[4] = (dataLen & 0xff);

				if (ScsiIO(fd, SG_DXFER_FROM_DEV, cdb, sizeof(cdb), sense, senseLen, data, dataLen))
				{
					int pageLen = data[3];

					TrimSerial(data + 4, (size_t)pageLen, serial);  /* 单位序列号 */
				}

				close(fd);

				return (!serial.empty());
			}

			static bool DiskSerialThirdMethod(const std::string & diskName, std::string & serial)
			{
				const char * file = ".hdparm_result.txt";

				std::string op = TINY_STR_FORMAT("hdparm -i {} 2>/dev/null | grep SerialNo > {}", diskName, file);

				if (::system(op.c_str()) == 0)
				{
					DiskSerialFromFile(file, "SerialNo=", serial);
				}

				unlink(file);  // 删除文件的目录项并减少它的链接数

				return (!serial.empty());
			}

			static bool DiskSerialFourthMethod(std::string & serial)
			{
				const char * resultFile = ".lshw_result.txt";

				std::string op = TINY_STR_TO_STRING("lshw -class disk 2>/dev/null | grep serial > ", resultFile);

				if (::system(op.c_str()) == 0)
				{
					DiskSerialFromFile(resultFile, "serial:", serial);
				}

				unlink(resultFile);  // 删除文件的目录项并减少它的链接数

				return (!serial.empty());
			}

			static bool ParseDiskSerial(const char * line, const std::size_t size,
										const char * matchWord, std::string & serial)
			{
				TINY_ASSERT(line, "line is nullptr")
				TINY_ASSERT(matchWord, "matchWord is nullptr")

				const char * serial_s = strstr(line, matchWord);

				if (serial_s == nullptr)
				{
					return false;
				}

				serial_s += strlen(matchWord);

				while (isspace(serial_s[0]) != 0)
				{
					++serial_s;
				}

				const char * serial_e = line + size;
				const char * comma = strchr(serial_s, ',');

				if (comma)
				{
					serial_e = comma;
				}

				while (serial_e > serial_s && (isspace(serial_e[-1]) != 0))
				{
					--serial_e;
				}

				if (serial_e <= serial_s)
				{
					return false;
				}

				std::string(serial_s, serial_e).swap(serial);

				return true;
			}

			static void TrimSerial(const void * str, size_t len, std::string & serial)
			{
				TINY_ASSERT(str, "str is nullptr")

				const auto * serial_s = static_cast<const char *>(str);
				const char * serial_e = serial_s + len;

				while (serial_s < serial_e)
				{
					if (isspace(serial_s[0]) != 0)
					{
						++serial_s;
					}
					else if (serial_e[-1] == '\0' || (isspace(serial_e[-1]) != 0))
					{
						--serial_e;
					}
					else
					{
						break;
					}
				}

				if (serial_s < serial_e)
				{
					std::string(serial_s, serial_e).swap(serial);
				}
			}

			static void CPUSerialFromFile(const char * fileName, const char * matchWord, std::string & serial)
			{
				TINY_ASSERT(fileName, "fileName is nullptr")
				TINY_ASSERT(matchWord, "matchWord is nullptr")

				std::ifstream iFile(fileName, std::ios::binary);

				if (!iFile.is_open())
				{
					return;
				}

				char line[4096] = { 0 };

				while (!iFile.eof())
				{
					iFile.getline(line, sizeof(line));

					if (!iFile.good())  // 检查文件流是否正常
					{
						break;
					}

					const char * ptr = matchWord ? strstr(line, matchWord) : nullptr;

					if (ptr == nullptr)
					{
						continue;
					}

					ptr += matchWord ? strlen(matchWord) : 0;

					while (ptr[0] != '\0')
					{
						if (ptr[0] != ' ')
						{
							serial.push_back(ptr[0]);
						}

						++ptr;
					}

					if (!serial.empty())
					{
						break;
					}
				}

				iFile.close();
			}

			static void DiskSerialFromFile(const char * fileName, const char * matchWord, std::string & serial)
			{
				TINY_ASSERT(fileName, "fileName is nullptr")
				TINY_ASSERT(matchWord, "matchWord is nullptr")

				std::ifstream ifs(fileName, std::ios::binary);

				if (!ifs.is_open())
				{
					return;
				}

				char line[4096] = {0};

				while (!ifs.eof())
				{
					ifs.getline(line, sizeof(line));

					if (!ifs.good())  // 检查文件流是否正常
					{
						break;
					}

					if (ifs.gcount() == 0)  // 返回最后一个非格式化的抽取方法读取的字符数
					{
						continue;
					}

					if (ParseDiskSerial(line, (size_t)(ifs.gcount() - 1), matchWord, serial))
					{
						break;
					}
				}

				ifs.close();
			}

			static void BoardSerialFromFile(const char * fileName, const char * matchWord, std::string & serial)
			{
				TINY_ASSERT(fileName, "fileName is nullptr")
				TINY_ASSERT(matchWord, "matchWord is nullptr")

				std::ifstream iFile(fileName, std::ios::binary);

				if (!iFile.is_open())
				{
					return;
				}

				char line[4096] = { 0 };

				while (!iFile.eof())
				{
					iFile.getline(line, sizeof(line));

					if (!iFile.good())  // 检查文件流是否正常
					{
						break;
					}

					const char * ptr = matchWord ? std::strstr(line, matchWord) : nullptr;

					if (ptr == nullptr)
					{
						continue;
					}

					ptr += matchWord ? strlen(matchWord) : 0;

					while (ptr[0] != '\0')
					{
						if (ptr[0] != ' ')
						{
							serial.push_back(ptr[0]);
						}

						++ptr;
					}

					// None是没有主板序列号
					// To be filled by O.E.M. 是代工
					if (serial == "None" || serial == "TobefilledbyO.E.M.")
					{
						serial.clear();

						continue;
					}

					if (!serial.empty())
					{
						break;
					}
				}

				iFile.close();
			}
		};
	}
}


#define TINY_SERIAL_CPU(value) tinyCore::system::Serial::CPUSerial(value)
#define TINY_SERIAL_DISK(value) tinyCore::system::Serial::DiskSerial(value)
#define TINY_SERIAL_BOARD(value) tinyCore::system::Serial::BoardSerial(value)


#endif // __TINY_CORE__SYSTEM__SERIAL__H__
