#include <string.h>
#include <finsh.h>
#include <stdio.h>

#include <lwip/pbuf.h>
#include <lwip/inet.h>
#include <rtthread.h>

#include "ethercat.h"

#include "osal.h"
#include "oshw.h"
#include "nuc980.h"
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
static char IOmap[4096];
typedef struct  __attribute__((__packed__))
{
	unsigned char  mode_byte;
	unsigned short control_word;
	long  dest_pos;
	unsigned short error_word;
	unsigned short status_word;
	long  cur_pos;
}SERVO_DATA_T;
typedef struct
{
	SERVO_DATA_T servo_data[3];
}SERVOS_T;

SERVOS_T *servos = (SERVOS_T *)IOmap;

void view_slave_data()
{
	hex_dump(IOmap,32);
}

static void echo_time()
{
	struct timeval tp;
	osal_gettimeofday(&tp, 0);
	printf("****cur time = %d,%03d,%03d(us)\n", tp.tv_sec,tp.tv_usec/1000,tp.tv_usec%1000);
}

#define EC_TIMEOUTMON 500

int expectedWKC;
boolean needlf;
volatile int wkc;
boolean inOP;
uint8 currentgroup = 0;

void simpletest(char *ifname)
{
    int i, j, oloop, iloop, chk;
    needlf = FALSE;
    inOP = FALSE;

   printf("Starting simple test\n");

   /* initialise SOEM, bind socket to ifname */
   if (ec_init(ifname))
   {
      printf("ec_init on %s succeeded.\n",ifname);
      /* find and auto-config slaves */


       if ( ec_config_init(FALSE) > 0 )
      {
         printf("%d slaves found and configured.\n",ec_slavecount);
			ec_config_map(&IOmap);
         ec_configdc();

         printf("Slaves mapped, state to SAFE_OP.\n");
         /* wait for all slaves to reach SAFE_OP state */
         ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 4);

         oloop = ec_slave[0].Obytes;
         if ((oloop == 0) && (ec_slave[0].Obits > 0)) oloop = 1;
         if (oloop > 8) oloop = 8;
         iloop = ec_slave[0].Ibytes;
         if ((iloop == 0) && (ec_slave[0].Ibits > 0)) iloop = 1;
         if (iloop > 8) iloop = 8;

         printf("segments : %d : %d %d %d %d\n",ec_group[0].nsegments ,ec_group[0].IOsegment[0],ec_group[0].IOsegment[1],ec_group[0].IOsegment[2],ec_group[0].IOsegment[3]);

         printf("Request operational state for all slaves\n");
         expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
         printf("Calculated workcounter %d\n", expectedWKC);
         ec_slave[0].state = EC_STATE_OPERATIONAL;
         /* send one valid process data to make outputs in slaves happy*/
         ec_send_processdata();
         ec_receive_processdata(EC_TIMEOUTRET);
         /* request OP state for all slaves */
         ec_writestate(0);
         chk = 200;
         /* wait for all slaves to reach OP state */
         do
         {
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
         }
         while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));
         if (ec_slave[0].state == EC_STATE_OPERATIONAL )
         {
            printf("Operational state reached for all slaves.\n");
            inOP = TRUE;
            /* cyclic loop */
			 osal_timert t;
			osal_timer_start(&t, 1000);
            for(i = 1; i <= 10; i++)
            {
               while(osal_timer_is_expired(&t)==FALSE);
				osal_timer_start(&t, 1000);
				echo_time();
				ec_send_processdata();
               wkc = ec_receive_processdata(EC_TIMEOUTRET);
					// wkc
					// LRD（读）	1
					// LWR（写）	1
					// LRW（读写）—读成功	1
					// LRW（读写）—写成功	2
					// LRW（读写）—全部完成	3
				//printf("~~~~WKC %d \n", wkc);
				if(wkc >= expectedWKC)
				{
					printf("Processdata cycle %4d, WKC %d , O:", i, wkc);

					for(j = 0 ; j < oloop; j++)
					{
						printf(" %2.2x", *(ec_slave[0].outputs + j));
					}

					printf(" I:");
					for(j = 0 ; j < iloop; j++)
					{
						printf(" %2.2x", *(ec_slave[0].inputs + j));
					}
					printf(" T:%"PRId64"\r",ec_DCtime);
					needlf = TRUE;
				}
				//osal_usleep(1000);

                }
                inOP = FALSE;
            }
            else
            {
                printf("Not all slaves reached operational state.\n");
                ec_readstate();
                for(i = 1; i<=ec_slavecount ; i++)
                {
                    if(ec_slave[i].state != EC_STATE_OPERATIONAL)
                    {
                        printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",
                            i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }
            printf("\nRequest init state for all slaves\n");
            ec_slave[0].state = EC_STATE_INIT;
            /* request INIT state for all slaves */
            ec_writestate(0);

	  }
        else
        {
            printf("No slaves found!\n");
        }
        printf("End simple test, close socket\n");
        /* stop SOEM, close socket */
        ec_close();
    }
    else
    {
        printf("No socket connection on %s\nExecute as root\n",ifname);
    }
}

int safe_SDOwrite(uint16 Slave, uint16 Index, uint8 SubIndex, int size, void *data)
{
	int wkc, cnt=0;
	do{
		wkc = ec_SDOwrite(Slave, Index, SubIndex, FALSE, size, data, EC_TIMEOUTRXM);
		cnt++;
	}while(wkc<=0 && cnt<10);
	return wkc;
}
int safe_SDCwrite_b(uint16 Slave, uint16 Index, uint8 SubIndex, uint8 b)
{
	return safe_SDOwrite(Slave, Index, SubIndex, 1, &b);
}
int safe_SDCwrite_w(uint16 Slave, uint16 Index, uint8 SubIndex, uint16 w)
{
	return safe_SDOwrite(Slave, Index, SubIndex, 2, &w);
}
int safe_SDCwrite_dw(uint16 Slave, uint16 Index, uint8 SubIndex, uint32 dw)
{
	return safe_SDOwrite(Slave, Index, SubIndex, 4, &dw);
}

int safe_SDOread(uint16 Slave, uint16 Index, uint8 SubIndex, int size, void *data)
{
	int wkc, cnt=0;
	do{
		wkc = ec_SDOread(Slave, Index, SubIndex, FALSE, &size, data, EC_TIMEOUTRXM);
	}while(wkc<=0 && cnt<10);
	return wkc;
}
int safe_SDOread_b(uint16 Slave, uint16 Index, uint8 SubIndex, uint8 b)
{
	return safe_SDOread(Slave, Index, SubIndex, 1, &b);
}
int safe_SDOread_w(uint16 Slave, uint16 Index, uint8 SubIndex, uint16 w)
{
	return safe_SDOread(Slave, Index, SubIndex, 2, &w);
}
int safe_SDOread_dw(uint16 Slave, uint16 Index, uint8 SubIndex, uint32 dw)
{
	return safe_SDOread(Slave, Index, SubIndex, 4, &dw);
}


void viewSDO(uint16_t slave, uint16_t index, uint16_t subindex, int bytes)
{
	uint32_t dw = 0;
	int wkc;
	safe_SDOread(slave, index, subindex, bytes, &dw);
	printf("SDO read=%s, SDO[0x%04x.%02x] = 0x%08x\n", wkc?"success":"fail",index, subindex, dw);
}

void process_data_config()
{
	u8_t     ind;

	for(int slave = 1; slave <= *ecx_context.slavecount; slave++)
	{
		//rpdo------------
		//1c12.0
		safe_SDCwrite_b(slave, 0x1c12, 0, 0);
		safe_SDCwrite_w(slave, 0x1c12, 1, 0x1600);

		//1600
		ind = 0;
		safe_SDCwrite_b(slave, 0x1600, 0, 0);
		safe_SDCwrite_dw(slave, 0x1600, ++ind, htoel(0x60600008));//6060h(控制模式)
		safe_SDCwrite_dw(slave, 0x1600, ++ind, htoel(0x60400010));//6040h(控制字)
		safe_SDCwrite_dw(slave, 0x1600, ++ind, htoel(0x607a0020));//607Ah(目标位置)
		safe_SDCwrite_b(slave, 0x1600, 0, ind);

		//1c12.0
		safe_SDCwrite_b(slave, 0x1c12, 0, 1);

		//tpdo-------------
		//1c13.0
		safe_SDCwrite_b(slave, 0x1c13, 0x00, 0);
		safe_SDCwrite_w(slave, 0x1c13, 0x01, 0x1a00);

		//1a00
		ind = 0;
		safe_SDCwrite_b(slave, 0x1a00, 0, 0);
		safe_SDCwrite_dw(slave, 0x1a00, ++ind, htoel(0x603F0010));//603Fh(错误码)
		safe_SDCwrite_dw(slave, 0x1a00, ++ind, htoel(0x60410010));//6041h(状态字)
		safe_SDCwrite_dw(slave, 0x1a00, ++ind, htoel(0x60640020));//6064h(位置反馈)
		safe_SDCwrite_b(slave, 0x1a00, 0, ind);

		//1c13.0
		safe_SDCwrite_b(slave, 0x1c13, 0, 1);

		safe_SDCwrite_b(slave, 0x6060, 0, 1);
		//viewSDO(slave, 0x6060, 0, 1);
	}
}


void servo_switch_op()
{
	int sta;
	for(int slave = 1; slave <= *ecx_context.slavecount; slave++)
	{
		int idx = slave - 1;
		sta = servos->servo_data[idx].status_word & 0x3ff;
		//printf("servo_switch_op: slave %d [6041]=%04x\n",slave,servos->servo_data[idx].status_word );

		if(servos->servo_data[idx].status_word & 0x8){
			servos->servo_data[idx].control_word = 0x8;
//			printf("***slave %d control=%04x\n",slave,servos->servo_data[idx].control_word );
			continue;
		}
		//printf("servo_switch_op: slave %d sta=%04x\n", slave, sta );
		switch(sta)
		{
			case 0x250:
			case 0x270:
				servos->servo_data[idx].control_word = 0x6;
				break;
			case 0x231:
				servos->servo_data[idx].control_word = 0x7;
				break;
			case 0x233:
				servos->servo_data[idx].control_word = 0xf;
				break;
			default:
				//servos->servo_data[idx].control_word = 0x6;
				break;
		}
		//printf("slave %d control=%04x\n",slave,servos->servo_data[idx].control_word );
	}

}
void servo_switch_idle()
{
	int sta;
	for(int slave = 1; slave <= *ecx_context.slavecount; slave++)
	{
		servos->servo_data[slave-1].control_word = 0x0;
	}
}
void sv660n_config(char *ifname)
{
	needlf = FALSE;
    inOP = FALSE;
	osal_timer_init();

	ecx_context.manualstatechange = 1;

	printf("========================\n");
	printf("sv660 config\n");
	echo_time();

	if (ec_init(ifname))
	{
		printf("ec_init on %s succeeded.\n",ifname);

		//init status
		printf("\nRequest init state for all slaves\n");
		ec_slave[0].state = EC_STATE_INIT;
		//request INIT state for all slaves
		ec_writestate(0);

		//显示1状态
		/* wait for all slaves to reach SAFE_OP state */
		ec_statecheck(0, EC_STATE_INIT,  EC_TIMEOUTSTATE * 3);
		if (ec_slave[0].state != EC_STATE_INIT ){
			printf("Not all slaves reached init state.\n");
			ec_readstate();
			for(int i = 1; i<=ec_slavecount ; i++){
				if(ec_slave[i].state != EC_STATE_INIT){
					printf("Slave %d State=0x%2x StatusCode=0x%04x : %s\n", i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
				}
			}
		}
		echo_time();
		//if ( ec_config(FALSE, &IOmap) > 0 )

		wkc = ec_config_init(0/*usetable*/);

		if (wkc > 0)
		{

			ec_configdc();
			ec_dcsync0(1, TRUE, 2000000, 50); // SYNC0 on slave 1
			while(EcatError) printf("%s", ec_elist2string());
			printf("%d slaves found and configured.\n",ec_slavecount);

			/* request pre_op for slave */
			printf("\nRequest pre_op state for all slaves\n");
			ec_slave[0].state = EC_STATE_PRE_OP | EC_STATE_ACK;
			ec_writestate(0);

			//故障复位
//			safe_SDCwrite_w(1,0x200d, 0x2, 1);
//			safe_SDCwrite_w(1,0x200d, 0x2, 0);


			//现在应该在pre_op状态
			//显示2状态
			process_data_config(); //config tpdo/rpdo

			//config fmmu
			ec_config_map(IOmap);


			/* request safe_op for slave */
			ec_slave[0].state = EC_STATE_SAFE_OP;
			ec_writestate(0);

			//safe-op
			expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
			printf("Calculated workcounter %d\n", expectedWKC);

			/* wait for all slaves to reach SAFE_OP state */
			ec_statecheck(0, EC_STATE_SAFE_OP,  EC_TIMEOUTSTATE * 3);
			if (ec_slave[0].state != EC_STATE_SAFE_OP ){
				printf("Not all slaves reached safe operational state.\n");
				ec_readstate();
				for(int i = 1; i<=ec_slavecount ; i++){
					if(ec_slave[i].state != EC_STATE_SAFE_OP){
						printf("Slave %d State=0x%2x StatusCode=0x%04x : %s\n", i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
					}
				}
			}else{
				//显示4状态
				//启动伺服
				servos->servo_data[0].mode_byte = 8; //csp mode

				//op status
				printf("Request operational state for all slaves\n");
				expectedWKC = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
				printf("Calculated workcounter %d\n", expectedWKC);
				ec_slave[0].state = EC_STATE_OPERATIONAL;
				// send one valid process data to make outputs in slaves happy
				ec_send_processdata();
				wkc = ec_receive_processdata(EC_TIMEOUTRET*3);
				printf("--->workcounter %d\n", wkc);
				//view_slave_data();
				// request OP state for all slaves
				ec_writestate(0);

				int chk = 200;
				// wait for all slaves to reach OP state
				do
				{
					ec_send_processdata();
					ec_receive_processdata(EC_TIMEOUTRET);
					printf("--->workcounter %d\n", wkc);
					ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
				}
				while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

				if (ec_slave[0].state == EC_STATE_OPERATIONAL )
				{
					printf("<<<Operational>>> state reached for all slaves.\n");
					inOP = TRUE;

					osal_timert t;
					osal_timer_start(&t, 1000);

					// cyclic loop
					for(int i = 1; i <= 10000; i++)
					{
						servo_switch_op();
						if(servos->servo_data[0].control_word==7){
							servos->servo_data[0].dest_pos = servos->servo_data[0].cur_pos;
							//printf("cur pos = %ld\n", servos->servo_data[0].cur_pos);
						}
						if(servos->servo_data[0].control_word==0xf){
							servos->servo_data[0].dest_pos += 3000;
						}
						while(osal_timer_is_expired(&t)==FALSE);
						osal_timer_start(&t, 1000);

						ec_send_processdata();
						wkc = ec_receive_processdata(EC_TIMEOUTRET);

						if(wkc >= expectedWKC){
							//printf("~~~~WKC %d \n", wkc);
						}
						if(wkc <=0 ){
							printf("Error.\n");
							break;
						}
						//osal_usleep(1000);
					}
					inOP = FALSE;
				}
				else
				{
					printf("Not all slaves reached operational state.\n");
					ec_readstate();
					for(int i = 1; i<=ec_slavecount ; i++)
					{
						if(ec_slave[i].state != EC_STATE_OPERATIONAL)
						{
							printf("Slave %d State=0x%2.2x StatusCode=0x%4.4x : %s\n",i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
						}
					}
				}

				//init status
				printf("\nRequest init state for all slaves\n");
				ec_slave[0].state = EC_STATE_INIT;
				//request INIT state for all slaves
				ec_writestate(0);
			}
		} else {
			printf("No slaves found!\n");
		}

		echo_time();
		printf("End soem, close socket\n");

		// stop SOEM, close socket
		ec_close();
	}else{
		printf("ec_init on %s failed.\n",ifname);
	}
	printf("IOMAP addr = 0x%08x\n", (uint32_t)IOmap);

	printf("========================\n");
	view_slave_data();
}

ALIGN(RT_ALIGN_SIZE)
static char thread_stack[4096];
static struct rt_thread thread;
#define THREAD_PRIORITY         15
//#define THREAD_STACK_SIZE     512
#define THREAD_TIMESLICE        50

static void thread_entry(void *param)
{
	sv660n_config("e0");
}

void ectest(int argc, char **argv)
{
//	osal_timer_init();
//	rt_thread_init(&thread,
//                   "thread_soem",
//                   thread_entry,
//                   RT_NULL,
//                   &thread_stack[0],
//                   sizeof(thread_stack),
//                   THREAD_PRIORITY,
//				   THREAD_TIMESLICE);
//    rt_thread_startup(&thread);

	//	if(argc<2){
//		printf("usage : ectest e0\n");
//		return;
//	}

	//view_slave_data();

	//config sv660n
	sv660n_config("e0");
	//simpletest("e0");

}

void sdo_test(int argc, char **argv)
{
	if(argc<2){
		printf("usage : sdo_test e0\n");
		return;
	}
	if (ec_init(argv[1]))
	{
		printf("ec_init on %s succeeded.\n",argv[1]);
		wkc = ec_config_init(0/*usetable*/);
		ec_close();
	}
}


MSH_CMD_EXPORT(ectest, sv660n ethercat comm sample);
