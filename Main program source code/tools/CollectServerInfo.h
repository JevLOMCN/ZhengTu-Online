/**
 * \file
 * \version  $Id: CollectServerInfo.h $
 * \author  
 * \date 
 * \brief 定义收集服务器信息类
 */

#ifndef	_CollectServerInfo_h_
#define	_CollectServerInfo_h_

#include <sys/sysinfo.h>

#include "zType.h"
#include "zSocket.h"
#include "zTime.h"
#include "zMisc.h"

/**
 * \brief 服务器信息采集类
 */
class ServerInfo
{

	public:

		/**
		 * \brief 定义网卡信息结构
		 */
		struct NetIf
		{
			/**
			 * \brief 字节数据
			 */
			struct BytesInfo
			{
				zTime tsp;						//时间戳
				unsigned long long rx_bytes;	//接收字节数
				unsigned long long tx_bytes;	//发送字节数

				/**
				 * \brief 缺省构造函数
				 */
				BytesInfo() : tsp()
				{
					rx_bytes = 0;
					tx_bytes = 0;
				}
				/**
				 * \brief 拷贝构造函数
				 * \param bi 待拷贝的数据
				 */
				BytesInfo(const BytesInfo &bi)
				{
					tsp = bi.tsp;
					rx_bytes = bi.rx_bytes;
					tx_bytes = bi.tx_bytes;
				}
				/**
				 * \brief 重载赋值操作符
				 * \param bi 待赋值的源数据
				 * \return 赋值以后结构的引用
				 */
				BytesInfo &operator= (const BytesInfo &bi)
				{
					tsp = bi.tsp;
					rx_bytes = bi.rx_bytes;
					tx_bytes = bi.tx_bytes;
					return *this;
				}
			};

			char name[MAX_NAMESIZE];		//网卡名称
			BytesInfo prev;					//上次扫描网卡信息
			BytesInfo current;				//当前扫描网卡信息
			float delta;					//两次扫描间隔时间
			unsigned long long rx_packets;	//当前接收的数据包数
			unsigned long long tx_packets;	//当前发送的数据包数
			float rx_speed;					//接收流量速度
			float rx_speed_max;				//接收流量速度最大值
			float tx_speed;					//发送流量速度
			float tx_speed_max;				//发送流量速度最大值

			/**
			 * \brief 缺省构造函数
			 */
			NetIf() : prev(), current()
			{
				bzero(name, sizeof(name));
				delta = 0.0;
				rx_packets = 0LL;
				tx_packets = 0LL;
				rx_speed = 0.0;
				rx_speed_max = 0.0;
				tx_speed = 0.0;
				tx_speed_max = 0.0;
			}
			/**
			 * \brief 拷贝构造函数
			 * \param net 源数据
			 */
			NetIf(const NetIf &net)
			{
				strcpy(name, net.name);
				prev = net.prev;
				current = net.current;
				delta = net.delta;
				rx_packets = net.rx_packets;
				tx_packets = net.tx_packets;
				rx_speed = net.rx_speed;
				rx_speed_max = net.rx_speed_max;
				tx_speed = net.tx_speed;
				tx_speed_max = net.tx_speed_max;
			}
			/**
			 * \brief 重载赋值操作符
			 * \param net 源数据
			 * \return 返回赋值以后结构本身引用
			 */
			NetIf &operator= (const NetIf &net)
			{
				strcpy(name, net.name);
				prev = net.prev;
				current = net.current;
				delta = net.delta;
				rx_packets = net.rx_packets;
				tx_packets = net.tx_packets;
				rx_speed = net.rx_speed;
				rx_speed_max = net.rx_speed_max;
				tx_speed = net.tx_speed;
				tx_speed_max = net.tx_speed_max;
				return *this;
			}
		};

		/**
		 * \brief CPU信息
		 */
		struct CpuInfo
		{
			/**
			 * \brief 单个CPU的信息
			 */
			struct Cpu
			{
				/**
				 * \brief 单个CPU的使用情况
				 */
				struct TickInfo
				{
					unsigned long long u,	//user: normal processes executing in user mode
								  n,		//nice: niced processes executing in user mode
								  s,		//system: processes executing in kernel mode
								  i,		//idle: twiddling thumbs
								  w,		//iowait: waiting for I/O to complete
								  x,		//irq: servicing interrupts
								  y;		//softirq: servicing softirqs
					/**
					 * \brief 构造函数
					 */
					TickInfo()
					{
						u = 0;
						n = 0;
						s = 0;
						i = 0;
						w = 0;
						x = 0;
						y = 0;
					}
					/**
					 * \brief 拷贝构造函数
					 * \param ti 拷贝构造的源数据
					 */
					TickInfo(const TickInfo &ti)
					{
						u = ti.u;
						n = ti.n;
						s = ti.s;
						i = ti.i;
						w = ti.w;
						x = ti.x;
						y = ti.y;
					}
					/**
					 * \brief 赋值运算符
					 * \param ti 待赋值的源数据
					 * \return 赋值以后结构的常量引用
					 */
					TickInfo &operator= (const TickInfo &ti)
					{
						u = ti.u;
						n = ti.n;
						s = ti.s;
						i = ti.i;
						w = ti.w;
						x = ti.x;
						y = ti.y;
						return *this;
					}
					/**
					 * \brief 根据已采集数据计算利用率
					 * \param pv 上一次采集的数据
					 * \param ct 当前采集的数据
					 */
					void calculate(const TickInfo &pv, const TickInfo &ct)
					{
						long long u_frme, s_frme, n_frme, i_frme, w_frme, x_frme, y_frme, tot_frme, tz;
						float scale;

						u_frme = ct.u - pv.u;
						s_frme = ct.s - pv.s;
						n_frme = ct.n - pv.n;
						i_frme = (tz = (long long)(ct.i - pv.i)) < 0 ? 0 : tz;
						w_frme = ct.w - pv.w;
						x_frme = ct.x - pv.x;
						y_frme = ct.y - pv.y;
						tot_frme = u_frme + s_frme + n_frme + i_frme + w_frme + x_frme + y_frme;
						if (tot_frme < 1) tot_frme = 1;
						scale = 1000.0 / (float)tot_frme;

						u = (unsigned long long)((float)u_frme * scale);
						n = (unsigned long long)((float)n_frme * scale);
						s = (unsigned long long)((float)s_frme * scale);
						i = (unsigned long long)((float)i_frme * scale);
						w = (unsigned long long)((float)w_frme * scale);
						x = (unsigned long long)((float)x_frme * scale);
						y = (unsigned long long)((float)y_frme * scale);
					}
				};

				/**
				 * \brief CPU编号
				 */
				const unsigned int id;
				/**
				 * \brief 上一次采集的数据
				 */
				TickInfo prev;
				/**
				 * \brief 当前数据
				 */
				TickInfo current;
				/**
				 * \brief 计算出来的CPU各项利用率
				 */
				TickInfo usage;
				/**
				 * \brief 构造函数
				 * \param id CPU编号
				 */
				explicit Cpu(const unsigned int id)
					: id(id), prev(), current(), usage() { }
				/**
				 * \brief 拷贝构造函数
				 * \param cpu 拷贝构造的源数据
				 */
				Cpu(const Cpu &cpu) : id(cpu.id)
				{
					prev = cpu.prev;
					current = cpu.current;
					usage = cpu.usage;
				}
				/**
				 * \brief 赋值操作符
				 * \param cpu 待赋值的源数据
				 * \return 赋值以后的数据常量引用
				 */
				Cpu &operator= (const Cpu &cpu)
				{
					prev = cpu.prev;
					current = cpu.current;
					usage = cpu.usage;
					return *this;
				}
			};

			/**
			 * \brief CPU数量
			 */
			const unsigned int CpuNum;
			/**
			 * \brief 各个CPU使用率的向量
			 */
			std::vector<Cpu> CpuUsage;
			/**
			 * \brief 定义大小类型
			 */
			typedef std::vector<Cpu>::size_type size_type;

			/**
			 * \brief 构造函数
			 * \param CpuNum CPU数量
			 */
			CpuInfo(unsigned int CpuNum = getCpuNum()) : CpuNum(CpuNum)
			{
				for(unsigned int i = 0; i <= CpuNum; i++)
				{
					Cpu cpu(i);
					CpuUsage.push_back(cpu);
				}
			}
		};

		/**
		 * \brief 定义网卡信息向量容器类型
		 */
		typedef std::vector<NetIf> NetIfInfo;

		/**
		 * \brief 返回CPU个数
		 * \return CPU个数
		 */
		static inline unsigned int getCpuNum()
		{
			return (unsigned int)sysconf(_SC_NPROCESSORS_ONLN);
		}

		int getCpuInfo();
		NetIfInfo::size_type getNetStat();
		void getServerInfo(std::string &xmlStr);

		CpuInfo cpuInfo;
		NetIfInfo if_vector;
		
};

typedef singleton_default<ServerInfo> ServerInfoSingleton;

#endif

