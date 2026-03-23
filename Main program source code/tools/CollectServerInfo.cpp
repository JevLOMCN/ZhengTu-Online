/**
 * \file
 * \version  $Id: CollectServerInfo.cpp  $
 * \author  
 * \date 
 * \brief 定义收集服务器信息类
 */

#include <errno.h>
#include <error.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
/////////////////////
/////////////////////
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "CollectServerInfo.h"
#include "zTime.h"
#include "zXMLParser.h"

/**
 * \brief 获取服务器所有信息
 * 这些信息包括网卡信息，内存信息，CPU信息，系统信息以及进程信息
 * 按照xml格式输出到一个数据缓冲
 */
void ServerInfo::getServerInfo(std::string &xmlStr)
{
	zXMLParser xml;
	xmlNodePtr root, node, snode;
	char buffer[512];
	if (xml.init()
			&& NULL != (root = xml.newRootNode("Server")))
	{
		node = xml.newChildNode(root, "SYS", NULL);
		if (node)
		{
			struct utsname uts;
			if (0 == uname(&uts))
			{
				xml.newNodeProp(node, "sysname", uts.sysname);
				xml.newNodeProp(node, "nodename", uts.nodename);
				xml.newNodeProp(node, "release", uts.release);
				xml.newNodeProp(node, "version", uts.version);
				xml.newNodeProp(node, "machine", uts.machine);
				xml.newNodeProp(node, "domainname", uts.domainname);
			}

			struct sysinfo si;
			if (0 == sysinfo(&si))
			{
				snprintf(buffer, sizeof(buffer) - 1, "%lu", time(NULL));
				xml.newNodeProp(node, "time", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%lu", si.uptime);
				xml.newNodeProp(node, "uptime", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%.2f, %.2f, %.2f", (float)si.loads[0] / 65536.0, (float)si.loads[1] / 65536.0, (float)si.loads[2] / 65536.0);
				xml.newNodeProp(node, "loads", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%u", si.procs);
				xml.newNodeProp(node, "processes", buffer);

				snode = xml.newChildNode(node, "MEM", NULL);
				if (snode)
				{
					snprintf(buffer, sizeof(buffer) - 1, "%u", si.mem_unit);
					xml.newNodeProp(snode, "unit", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%lu", si.totalram);
					xml.newNodeProp(snode, "totalram", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%lu", si.freeram);
					xml.newNodeProp(snode, "freeram", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%lu", si.sharedram);
					xml.newNodeProp(snode, "sharedram", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%lu", si.bufferram);
					xml.newNodeProp(snode, "bufferram", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%lu", si.totalswap);
					xml.newNodeProp(snode, "totalswap", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%lu", si.freeswap);
					xml.newNodeProp(snode, "freeswap", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%lu", si.totalhigh);
					xml.newNodeProp(snode, "totalhigh", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%lu", si.freehigh);
					xml.newNodeProp(snode, "freehigh", buffer);
				}
			}
		}

		getCpuInfo();
		node = xml.newChildNode(root, "CPUTS", NULL);
		if (node)
		{
			snprintf(buffer, sizeof(buffer) - 1, "%u", cpuInfo.CpuNum);
			xml.newNodeProp(node, "total", buffer);
			snode = xml.newChildNode(node, "cpus", NULL);
			if (snode)
			{
				snprintf(buffer, sizeof(buffer) - 1, "%u", cpuInfo.CpuNum);
				xml.newNodeProp(snode, "id", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[cpuInfo.CpuNum].usage.u / 10.0);
				xml.newNodeProp(snode, "user", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[cpuInfo.CpuNum].usage.n / 10.0);
				xml.newNodeProp(snode, "nice", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[cpuInfo.CpuNum].usage.s / 10.0);
				xml.newNodeProp(snode, "system", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[cpuInfo.CpuNum].usage.i / 10.0);
				xml.newNodeProp(snode, "idle", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[cpuInfo.CpuNum].usage.w / 10.0);
				xml.newNodeProp(snode, "iowait", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[cpuInfo.CpuNum].usage.x / 10.0);
				xml.newNodeProp(snode, "irq", buffer);
				snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[cpuInfo.CpuNum].usage.y / 10.0);
				xml.newNodeProp(snode, "softirq", buffer);
			}
			for(CpuInfo::size_type i = 0; i < cpuInfo.CpuNum; i++)
			{
				snprintf(buffer, sizeof(buffer) - 1, "%s", "cpu");
				snode = xml.newChildNode(node, buffer, NULL);
				if (snode)
				{
					snprintf(buffer, sizeof(buffer) - 1, "%zu", i);
					xml.newNodeProp(snode, "id", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[i].usage.u / 10.0);
					xml.newNodeProp(snode, "user", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[i].usage.n / 10.0);
					xml.newNodeProp(snode, "nice", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[i].usage.s / 10.0);
					xml.newNodeProp(snode, "system", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[i].usage.i / 10.0);
					xml.newNodeProp(snode, "idle", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[i].usage.w / 10.0);
					xml.newNodeProp(snode, "iowait", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[i].usage.x / 10.0);
					xml.newNodeProp(snode, "irq", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float)cpuInfo.CpuUsage[i].usage.y / 10.0);
					xml.newNodeProp(snode, "softirq", buffer);
				}
			}
		}

		getNetStat();
		node = xml.newChildNode(root, "NET", NULL);
		if (node)
		{
			snprintf(buffer, sizeof(buffer) - 1, "%zu", if_vector.size());
			xml.newNodeProp(node, "total", buffer);
			for(NetIfInfo::size_type i = 0; i < if_vector.size(); i++)
			{
				snprintf(buffer, sizeof(buffer) - 1, "%s", if_vector[i].name);
				snode = xml.newChildNode(node, buffer, NULL);
				if (snode)
				{
					snprintf(buffer, sizeof(buffer) - 1, "%s", zSocket::getIPByIfName(if_vector[i].name));
					xml.newNodeProp(snode, "addr", buffer);
					snprintf(buffer, sizeof(buffer) - 1, "%.0f", if_vector[i].delta);
					xml.newNodeProp(snode, "delta", buffer);
					/*
					   if (if_vector[i].rx_speed > (1024 * 1024))
					   snprintf(buffer, sizeof(buffer) - 1, "%.2fM", if_vector[i].rx_speed / (1024 * 1024));
					   else if (if_vector[i].rx_speed > (1024))
					   snprintf(buffer, sizeof(buffer) - 1, "%.2fK", if_vector[i].rx_speed / (1024));
					   else
					   */
					snprintf(buffer, sizeof(buffer) - 1, "%.0f", if_vector[i].rx_speed);
					xml.newNodeProp(snode, "recv", buffer);
					/*
					   if (if_vector[i].tx_speed > (1024 * 1024))
					   snprintf(buffer, sizeof(buffer) - 1, "%.2fM", if_vector[i].tx_speed / (1024 * 1024));
					   else if (if_vector[i].tx_speed > (1024))
					   snprintf(buffer, sizeof(buffer) - 1, "%.2fK", if_vector[i].tx_speed / (1024));
					   else
					   */
					snprintf(buffer, sizeof(buffer) - 1, "%.0f", if_vector[i].tx_speed);
					xml.newNodeProp(snode, "transmit", buffer);
				}
			}
		}
		
		xml.dump(xmlStr, true);
		//Zebra::logger->debug("%s", xmlStr.c_str());
	}
}

/**
 * \brief 获得各CPU使用率
 * \return CUP的数量
 */
int ServerInfo::getCpuInfo()
{
	FILE *fp = NULL;
	char buff[1024];

	fp = fopen ("/proc/stat", "r");

	if (fp)
	{
		if (fgets(buff, 128, fp)) 
		{
			cpuInfo.CpuUsage[cpuInfo.CpuNum].prev = cpuInfo.CpuUsage[cpuInfo.CpuNum].current;
			sscanf(buff,
					"cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu\n",
					&cpuInfo.CpuUsage[cpuInfo.CpuNum].current.u, &cpuInfo.CpuUsage[cpuInfo.CpuNum].current.n,
					&cpuInfo.CpuUsage[cpuInfo.CpuNum].current.s, &cpuInfo.CpuUsage[cpuInfo.CpuNum].current.i,
					&cpuInfo.CpuUsage[cpuInfo.CpuNum].current.w, &cpuInfo.CpuUsage[cpuInfo.CpuNum].current.x, &cpuInfo.CpuUsage[cpuInfo.CpuNum].current.y);
			cpuInfo.CpuUsage[cpuInfo.CpuNum].usage.calculate(cpuInfo.CpuUsage[cpuInfo.CpuNum].prev, cpuInfo.CpuUsage[cpuInfo.CpuNum].current);
		}
		for(unsigned int i = 0; i < cpuInfo.CpuNum; i++) 
		{
			if (fgets(buff, 128, fp)) 
			{
				cpuInfo.CpuUsage[i].prev = cpuInfo.CpuUsage[i].current;
				sscanf(buff,
						"cpu%*d %Lu %Lu %Lu %Lu %Lu %Lu %Lu\n",
						&cpuInfo.CpuUsage[i].current.u, &cpuInfo.CpuUsage[i].current.n,
						&cpuInfo.CpuUsage[i].current.s, &cpuInfo.CpuUsage[i].current.i,
						&cpuInfo.CpuUsage[i].current.w, &cpuInfo.CpuUsage[i].current.x, &cpuInfo.CpuUsage[i].current.y);
				cpuInfo.CpuUsage[i].usage.calculate(cpuInfo.CpuUsage[i].prev, cpuInfo.CpuUsage[i].current);
			}
		}
		fclose (fp);
	}

	return cpuInfo.CpuNum;
}

/**
 * \brief 获得各网卡流量
 * \return 获取到的网卡信息数量
 */
ServerInfo::NetIfInfo::size_type ServerInfo::getNetStat()
{
	FILE *fp;
	char buff[1024];

	fp = fopen("/proc/net/dev", "r");
	if (fp)
	{
		fgets(buff, 256, fp);	// skip the header, two lines
		fgets(buff, 256, fp);

		std::vector<NetIf>::size_type i = 0;
		while(true)
		{
			if (if_vector.size() > i)
			{
				NetIf &net = if_vector[i];
				zTime ct;
				net.delta = (float)net.current.tsp.elapse(ct);
				net.prev = net.current;
				net.current.tsp = ct;
				if (fgets(buff, 256, fp))
				{
					char *b = buff;
					while(*b == ' ')
						b++;
					int j = 0;
					while(*b != ':' && j < (16 - 1))
						net.name[j++] = *b++;
					b++;
					sscanf(b,
							"%Lu %Lu %*d %*d %*d %*d %*d %*d %Lu %Lu",
							&net.current.rx_bytes, &net.rx_packets,
							&net.current.tx_bytes, &net.tx_packets);
					if (net.current.rx_bytes > net.prev.rx_bytes && net.delta > 0.0)
						net.rx_speed = (float)(net.current.rx_bytes - net.prev.rx_bytes) / net.delta;
					else
						net.rx_speed = 0.0;
					if (net.rx_speed > net.rx_speed_max)
						net.rx_speed_max = net.rx_speed;
					if (net.current.tx_bytes > net.prev.tx_bytes && net.delta > 0.0)
						net.tx_speed = (float)(net.current.tx_bytes - net.prev.tx_bytes) / net.delta;
					else
						net.tx_speed = 0.0;
					if (net.tx_speed > net.tx_speed_max)
						net.tx_speed_max = net.tx_speed;
				}
				else
					break;
			}
			else
			{
				NetIf net;
				if (fgets(buff, 256, fp))
				{
					char *b = buff;
					while(*b == ' ')
						b++;
					int j = 0;
					while(*b != ':' && j < (16 - 1))
						net.name[j++] = *b++;
					b++;
					sscanf(b,
							"%Lu %Lu %*d %*d %*d %*d %*d %*d %Lu %Lu",
							&net.current.rx_bytes, &net.rx_packets,
							&net.current.tx_bytes, &net.tx_packets);
					if_vector.push_back(net);
				}
				else
					break;
			}

			i++;
		}
		fclose (fp);
	}

	return if_vector.size();
}

