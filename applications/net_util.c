/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#include <rtthread.h>

//#include <stddef.h>
#include <netdev_ipaddr.h>

#include <netdev.h>
#include "netdev_hook.h"

void net_util_enable_dhcp(char *ifname)
{
	struct netdev *netdev = netdev_get_by_name(ifname);
	if (netdev == RT_NULL)
        return;

	netdev_dhcp_enabled(netdev,RT_TRUE);
}

void net_util_set_static_ip(char *ifname, char *ip, char *gw, char *mask)
{
	ip_addr_t addr;
    struct netdev *netdev = netdev_get_by_name(ifname);
	if (netdev == RT_NULL)
        return;

	netdev_dhcp_enabled(netdev,RT_FALSE);

    /* set IP address */
    if ((ip != RT_NULL) && inet_aton(ip, &addr))
    {
        netdev_set_ipaddr(netdev, &addr);
    }

    /* set gateway address */
    if ((gw != RT_NULL) && inet_aton(gw, &addr))
    {
        netdev_set_gw(netdev, &addr);
    }

    /* set netmask address */
    if ((mask != RT_NULL) && inet_aton(mask, &addr))
    {
        netdev_set_netmask(netdev, &addr);
    }
}

void if_enable_dhcp(int argc, char **argv)
{
	if(argc!=2){
		rt_kprintf("bad parameter! e.g: if_enable_dhcp e0\n");
		return;
	}
	net_util_enable_dhcp(argv[1]);
}

void if_set_ip(int argc, char **argv)
{
	if(argc!=5){
		rt_kprintf("bad parameter! e.g: if_set_static_ip e0 192.168.1.30 192.168.1.1 255.255.255.0\n");
		return;
	}
    
    net_util_set_static_ip(argv[1], argv[2], argv[3], argv[4]);
}


MSH_CMD_EXPORT(if_enable_dhcp, start net device dhcp);
MSH_CMD_EXPORT(if_set_ip, set net device ip/gw/netmask);