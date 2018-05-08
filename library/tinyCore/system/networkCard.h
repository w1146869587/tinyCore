#ifndef __TINY_CORE__SYSTEM__NETWORK__H__
#define __TINY_CORE__SYSTEM__NETWORK__H__


/**
 *
 *  作者: hm
 *
 *  说明: 网卡管理
 *
 */


#include <tinyCore/debug/trace.h>


namespace tinyCore
{
	namespace system
	{
		typedef struct NETWORK_CARD
		{
			int32_t mtu;
			int32_t flags;
			int32_t index;
			int32_t metric;

			std::string mac;
			std::string host;
			std::string name;
			std::string mask;
			std::string remote;
			std::string broadcast;
		}NETWORK_CARD;

		using NETWORK_CARD_INFO = std::map<std::string, NETWORK_CARD>;

		class NetWork
		{
		public:
			static bool GetInfo(NETWORK_CARD_INFO & res)
			{
				int32_t fd = socket(AF_INET, SOCK_DGRAM, 0);

				if (fd < 0)
				{
					TINY_WARNING(strerror(errno));

					close(fd);

					return false;
				}

				struct if_nameindex * list = if_nameindex();
				struct if_nameindex * head = list;

				if (list == nullptr)
				{
					TINY_WARNING(strerror(errno));

					close(fd);

					return false;
				}

				struct ifreq ifr = { 0 };

				while (list->if_index != 0)
				{
					NETWORK_CARD info = { };

					if (ifr.ifr_name && list->if_name)
					{
						memcpy(ifr.ifr_name, list->if_name, strlen(list->if_name));

						info.name = list->if_name;
					}

					if (ioctl(fd, SIOCGIFMTU, &ifr) >= 0)
					{
						info.mtu = ifr.ifr_mtu;
					}

					if (ioctl(fd, SIOCGIFFLAGS, &ifr) >= 0)
					{
						info.flags = ifr.ifr_flags;
					}

					if (ioctl(fd, SIOCGIFINDEX, &ifr) >= 0)
					{
						info.index = ifr.ifr_ifindex;
					}

					if (ioctl(fd, SIOCGIFMETRIC, &ifr) >= 0)
					{
						info.metric = ifr.ifr_metric;
					}

					if (ioctl(fd, SIOCGIFADDR, &ifr) >= 0)
					{
						info.host = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
					}

					if (ioctl(fd, SIOCGIFNETMASK, &ifr) >= 0)
					{
						info.mask = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_netmask))->sin_addr);
					}

					if (ioctl(fd, SIOCGIFDSTADDR, &ifr) >= 0)
					{
						info.remote = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_dstaddr))->sin_addr);
					}

					if (ioctl(fd, SIOCGIFBRDADDR, &ifr) >= 0)
					{
						info.broadcast = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_broadaddr))->sin_addr);
					}

					if (ioctl(fd, SIOCGIFHWADDR, &ifr) >= 0)
					{
						if (ifr.ifr_hwaddr.sa_data)
						{
							info.mac = TINY_STR_FORMAT("{}:{}:{}:{}:{}:{}",
													   TINY_DIGITAL_TO_HEX_STR((uint8_t)ifr.ifr_hwaddr.sa_data[0]),
													   TINY_DIGITAL_TO_HEX_STR((uint8_t)ifr.ifr_hwaddr.sa_data[1]),
													   TINY_DIGITAL_TO_HEX_STR((uint8_t)ifr.ifr_hwaddr.sa_data[2]),
													   TINY_DIGITAL_TO_HEX_STR((uint8_t)ifr.ifr_hwaddr.sa_data[3]),
													   TINY_DIGITAL_TO_HEX_STR((uint8_t)ifr.ifr_hwaddr.sa_data[4]),
													   TINY_DIGITAL_TO_HEX_STR((uint8_t)ifr.ifr_hwaddr.sa_data[5]));
						}
					}

					res.insert(std::make_pair(info.name, info));

					++list;
				}

				close(fd);

				if_freenameindex(head);

				return true;
			}
		};
	}
}


#define TINY_NETWORK_INFO(res) tinyCore::system::NetWork::GetInfo(res)


#endif // __TINY_CORE__SYSTEM__NETWORK__H__

