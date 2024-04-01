#include <rtthread.h>

#include "netif/ethernetif.h"
#include "nicdrv.h"

#include <drv_gpio.h>
#include <rtdevice.h>


/******************************************************************************
* receive fifo buf
*/
#define HOOK_RX_BUFSIZE 10

static uint8_t netfrmbuf[HOOK_RX_BUFSIZE][1540];
static int netfrmbuf_cnt[HOOK_RX_BUFSIZE];
static int netfrm_head = 0;
static int netfrm_tail = 0;
static int netfrm_full = 0;

int hook_rx_dump = 0;
int hook_tx_dump = 0;

static inline void tx_led_flash(void)
{
	static int c = 0;
	//rt_kprintf("c = %d\n",c);
	if(++c%2)
		rt_pin_write(NU_GET_PININDEX(NU_PB, 8), PIN_LOW);
	else
		rt_pin_write(NU_GET_PININDEX(NU_PB, 8), PIN_HIGH);
}


/******************************************************************************
* store netif and old function addr
*/
static struct netif *netif = RT_NULL;
static netif_linkoutput_fn link_output;
static netif_input_fn input;

/******************************************************************************
* hex dump 
*/
#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
static void hex_dump(const rt_uint8_t *ptr, rt_size_t buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    int i, j;

    RT_ASSERT(ptr != RT_NULL);

    for (i = 0; i < buflen; i += 16)
    {
        rt_kprintf("%08X: ", i);

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                rt_kprintf("%02X ", buf[i + j]);
            else
                rt_kprintf("   ");
        rt_kprintf(" ");

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                rt_kprintf("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
        rt_kprintf("\n");
    }
}

/******************************************************************************
* rx/tx hook function 
*/
/* get tx data */
static err_t _netif_linkoutput(struct netif *netif, struct pbuf *p)
{
	return link_output(netif, p);
}

/* get rx data */
static err_t _netif_input(struct pbuf *p, struct netif *inp)
{
	if(p->tot_len>=14)
	{
		char *data = p->payload;
		if(data[12]=='\x88' && data[13]=='\xa4') //filter for ethercat frame
		{
			if(netfrm_full == 0){
				pbuf_copy_partial(p, netfrmbuf[netfrm_tail], p->tot_len, 0);
				netfrmbuf_cnt[netfrm_tail] = p->tot_len;
				netfrm_tail = (netfrm_tail+1) % HOOK_RX_BUFSIZE;
				if(netfrm_tail==netfrm_head)
					netfrm_full = 1;
			}
			//rt_kprintf("tail = %d, full = %d\n", netfrm_tail, netfrm_full);
		}
	}
    return input(p, inp);
}

/******************************************************************************
* hook install
*/
int install_hook(ecx_portt *port, const char *ifname)
{
	struct eth_device *device;
    rt_base_t level;
	
	netfrm_head = 0;
	netfrm_tail = 0;
	netfrm_full = 0;
	
	if(netif == RT_NULL){
		rt_kprintf("hook installing on  %s\n", ifname);
		device = (struct eth_device *)rt_device_find(ifname);
		if (device == RT_NULL){
			rt_kprintf("hook install error 'device == RT_NULL'\n");
			return 0;
		}
		if ((device->netif == RT_NULL) || (device->netif->linkoutput == RT_NULL)){
			rt_kprintf("hook install error '(device->netif == RT_NULL) || (device->netif->linkoutput == RT_NULL)'\n");
			return 0;
		}
		rt_kprintf("device %s found: 0x%x \n", ifname, (uint32_t)device);
	}else{
		rt_kprintf("device %s hook already installed, must be uninstall it before intall new on\n", ifname);
	}
    netif = device->netif;
	
    //install netdev hook
    level = rt_hw_interrupt_disable();
    link_output = netif->linkoutput;
    netif->linkoutput = _netif_linkoutput;

    input = netif->input;
    netif->input = _netif_input;
    rt_hw_interrupt_enable(level);
	rt_kprintf("hook installed on %s\n", ifname);

	return 1;
}

/******************************************************************************
* hook uninstall
*/
int uninstall_hook(ecx_portt *port)
{
    rt_base_t level;
	
	//uninstall netdev hook
	if(netif != RT_NULL){
		level = rt_hw_interrupt_disable();
		netif->input = input;
		netif->linkoutput = link_output;
		rt_hw_interrupt_enable(level);
		netif = RT_NULL;
	}
	
	rt_kprintf("hook uninstalled\n");
	return 1;
}

/******************************************************************************
* netdev send/recv api
*/
int net_send(unsigned char *data, int len)
{
	int ret = -1;
	struct pbuf *p;
	
tx_led_flash();
	
    p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL);
    
    if (p != NULL)
    {
		pbuf_take(p, data, len);
		
		if(hook_tx_dump){
			rt_kprintf("send --- len=%d>>>\n",len);
			hex_dump(p->payload, p->tot_len);
		}
		
		_netif_linkoutput(netif,p);
        pbuf_free(p);
		ret = len;
	}
	else{
		rt_kprintf("net_send alloc buffer error\n"); 
	}
	
	return ret;
}

int net_recv(unsigned char *data, int len)
{
	if(netfrm_full == 0 && netfrm_tail==netfrm_head){
		return 0;
	}
	
	int total = netfrmbuf_cnt[netfrm_head];
	if(total > len) total = len;
	
	rt_memcpy(data, netfrmbuf[netfrm_head], total);
	netfrm_head = (netfrm_head+1) % HOOK_RX_BUFSIZE;
	if(netfrm_tail==netfrm_head)
		netfrm_full = 0;

	if(hook_rx_dump){
		rt_kprintf("recv <<<---\n");
		hex_dump(data, total);
		rt_kprintf("head = %d, tail = %d, full = %d\n", netfrm_head, netfrm_tail, netfrm_full);
	}
	return total;
}
void net_hook_test(int argc, char **argv)
{
	unsigned char frame[] = "\xff\xff\xff\xff\xff\xff\x01\x01\x01\x01\x01\x01\x88\xa4\x0d\x10\
\x08\x01\x00\x00\x03\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

	install_hook(NULL, "e0");
	net_send(frame,60);
	rt_thread_mdelay(20000);
	uninstall_hook(NULL);
}
MSH_CMD_EXPORT(net_hook_test, net_hook_test sample on 'e0');
