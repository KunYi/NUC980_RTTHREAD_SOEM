/**************************************************************************//**
*
* @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date            Author       Notes
* 2020-12-12      Wayne        First version
*
******************************************************************************/

#include "board.h"
/******
 * UART
*/
#define UART0_PF11_PF12

//#define UART1_PF9_PF10
//#define UART1_PC6_PC5
//#define UART1_PA0_PA1

//#define UART2_PD7_PD6
//#define UART2_PG0_PG1
//#define UART2_PA9_PA10

//#define UART3_PD3_PD2
//#define UART3_PB10_PB9
//#define UART3_PC4_PC3
//#define UART3_PF6_PF7

//#define UART4_PD13_PD12
//#define UART4_PC10_PC9
//#define UART4_PE2_PE3

//#define UART5_PG6_PG7
//#define UART5_PD1_PD0
//#define UART5_PG13_PG14

//#define UART6_PD11_PD10
//#define UART6_PE8_PE9
//#define UART6_PA4_PA5

//#define UART7_PA14_PA13
//#define UART7_PB4_PB6
//#define UART7_PC2_PC1
//#define UART7_PF2_PF3

//#define UART8_PA11_PA12
//#define UART8_PC0_PB8
//#define UART8_PC13_PC12

//#define UART9_PB3_PB1
//#define UART9_PE6_PE7
//#define UART9_PE10_PE12

/******
 * EMAC
*/
#define EMAC0_PE
#define EMAC1_PF


static void nu_pin_uart_init(void)
{
/*******
UART0
*/
#ifdef UART0_PF11_PF12    
    outpw(REG_SYS_GPF_MFPH, (inpw(REG_SYS_GPF_MFPH) & 0xFFF00FFF) | 0x00011000);
#endif

/*******
UART1
*/
#ifdef UART1_PF9_PF10
    outpw(REG_SYS_GPF_MFPH, (inpw(REG_SYS_GPF_MFPH) & 0xFFFFF00F) | 0x00000220);
#endif
#ifdef UART1_PC6_PC5
    outpw(REG_SYS_GPC_MFPL, (inpw(REG_SYS_GPC_MFPL) & 0xF00FFFFF) | 0x07700000);
#endif
#ifdef UART1_PA0_PA1
    outpw(REG_SYS_GPA_MFPL, (inpw(REG_SYS_GPA_MFPL) & 0xFFFFFF00) | 0x00000044);
#endif

/*******
UART2
*/
#ifdef UART2_PD7_PD6
    outpw(REG_SYS_GPD_MFPL, (inpw(REG_SYS_GPD_MFPL) & 0x00FFFFFF) | 0x22000000);
#endif
#ifdef UART2_PG0_PG1
    outpw(REG_SYS_GPG_MFPL, (inpw(REG_SYS_GPG_MFPL) & 0xFFFFFF00) | 0x00000022);
#endif
#ifdef UART2_PA9_PA10
    outpw(REG_SYS_GPA_MFPH, (inpw(REG_SYS_GPA_MFPH) & 0xFFFFF00F) | 0x00000220);
#endif

/*******
UART3
*/
#ifdef UART3_PD3_PD2
    outpw(REG_SYS_GPD_MFPL, (inpw(REG_SYS_GPD_MFPL) & 0xFFFF00FF) | 0x00002200);
#endif
#ifdef UART3_PB10_PB9
    outpw(REG_SYS_GPB_MFPL, (inpw(REG_SYS_GPB_MFPH) & 0xFFFFF00F) | 0x00000110);
#endif
#ifdef UART3_PC4_PC3
    outpw(REG_SYS_GPC_MFPL, (inpw(REG_SYS_GPC_MFPL) & 0xFFF00FFF) | 0x00055000);
#endif
#ifdef UART3_PF6_PF7
    outpw(REG_SYS_GPF_MFPL, (inpw(REG_SYS_GPF_MFPL) & 0x00FFFFFF) | 0x55000000);
#endif

/*******
UART4
*/
#ifdef UART4_PD13_PD12
    outpw(REG_SYS_GPD_MFPH, (inpw(REG_SYS_GPD_MFPH) & 0xFF00FFFF) | 0x00110000);
#endif
#ifdef UART4_PE2_PE3
    outpw(REG_SYS_GPE_MFPL, (inpw(REG_SYS_GPE_MFPL) & 0xFFFF00FF) | 0x00005500);
#endif
#ifdef UART4_PC10_PC9
    outpw(REG_SYS_GPC_MFPH, (inpw(REG_SYS_GPC_MFPH) & 0xFFFFF00F) | 0x00000770);
#endif

/*******
UART5
*/
#ifdef UART5_PG6_PG7
    outpw(REG_SYS_GPG_MFPL, (inpw(REG_SYS_GPG_MFPL) & 0x00FFFFFF) | 0x22000000);
#endif
#ifdef UART5_PD1_PD0
    outpw(REG_SYS_GPD_MFPL, (inpw(REG_SYS_GPD_MFPL) & 0xFFFFFF00) | 0x00000022);
#endif
#ifdef UART5_PG13_PG14
    outpw(REG_SYS_GPG_MFPH, (inpw(REG_SYS_GPG_MFPH) & 0xF00FFFFF) | 0x05500000);
#endif

/******
UART6
*/
#ifdef UART6_PD11_PD10
    outpw(REG_SYS_GPD_MFPH, (inpw(REG_SYS_GPD_MFPH) & 0xFFFF00FF) | 0x00002200);
#endif
#ifdef UART6_PE8_PE9
    outpw(REG_SYS_GPE_MFPH, (inpw(REG_SYS_GPE_MFPH) & 0xFFFFFF00) | 0x00000055);
#endif
#ifdef UART6_PA4_PA5
    outpw(REG_SYS_GPA_MFPL, (inpw(REG_SYS_GPA_MFPL) & 0xFF00FFFF) | 0x00110000);
#endif


/*******
UART7
*/
#ifdef UART7_PA14_PA13
    outpw(REG_SYS_GPA_MFPH, (inpw(REG_SYS_GPA_MFPH) & 0xF00FFFFF) | 0x06600000);
#endif
#ifdef UART7_PB4_PB6
    outpw(REG_SYS_GPB_MFPL, (inpw(REG_SYS_GPB_MFPL) & 0xFF00FFFF) | 0x00550000);
#endif
#ifdef UART7_PC2_PC1
    outpw(REG_SYS_GPC_MFPL, (inpw(REG_SYS_GPC_MFPL) & 0xFFFFFF00) | 0x00000044);
#endif
#ifdef UART7_PF2_PF3
    outpw(REG_SYS_GPF_MFPL, (inpw(REG_SYS_GPF_MFPL) & 0xFFFF00FF) | 0x00005500);
#endif
/*******
UART8
*/
#ifdef UART8_PA11_PA12
    outpw(REG_SYS_GPA_MFPH, (inpw(REG_SYS_GPA_MFPH) & 0xFFF00FFF) | 0x00022000);
#endif
#ifdef UART8_PC0_PB8
    outpw(REG_SYS_GPC_MFPL, (inpw(REG_SYS_GPC_MFPL) & 0xFFFFFFF0) | 0x00000004);
    outpw(REG_SYS_GPB_MFPH, (inpw(REG_SYS_GPB_MFPH) & 0xFFFFFFF0) | 0x00000004);
#endif
#ifdef UART8_PC13_PC12
    outpw(REG_SYS_GPC_MFPH, (inpw(REG_SYS_GPC_MFPH) & 0xFF00FFFF) | 0x00770000);
#endif

/*******
UART9
*/
#ifdef UART9_PB3_PB1
    outpw(REG_SYS_GPB_MFPL, (inpw(REG_SYS_GPB_MFPL) & 0xFFFF0F0F) | 0x00007070);
#endif
#ifdef UART9_PE6_PE7
    outpw(REG_SYS_GPE_MFPL, (inpw(REG_SYS_GPE_MFPL) & 0x00FFFFFF) | 0x55000000);
#endif
#ifdef UART9_PE10_PE12
    outpw(REG_SYS_GPC_MFPH, (inpw(REG_SYS_GPC_MFPH) & 0xFFF0F0FF) | 0x00030300);
#endif
}

static void nu_pin_emac_init(void)
{
#ifdef EMAC0_PE
    /* EMAC0  */
    outpw(REG_SYS_GPE_MFPL, 0x11111111);
    outpw(REG_SYS_GPE_MFPH, (inpw(REG_SYS_GPE_MFPH) & 0xFFFFFF00) | 0x00000011);
#endif

#ifdef EMAC1_PF
    /* EMAC1  */
    outpw(REG_SYS_GPF_MFPL, 0x11111111);
    outpw(REG_SYS_GPF_MFPH, (inpw(REG_SYS_GPF_MFPH) & 0xFFFFFF00) | 0x00000011);
#endif
}

//static void nu_pin_uart_init(void)
//{
//    /* UART0: GPF11, GPF12 */
//    outpw(REG_SYS_GPF_MFPH, (inpw(REG_SYS_GPF_MFPH) & 0xFFF00FFF) | 0x00011000);

//    /* UART4: GPC9, GPC10 */
//    outpw(REG_SYS_GPC_MFPH, (inpw(REG_SYS_GPC_MFPH) & 0xFFFFF00F) | 0x00000770);

//    /* UART8: GPC12, GPC13, GPC14 */
//    outpw(REG_SYS_GPC_MFPH, (inpw(REG_SYS_GPC_MFPH) & 0xF000FFFF) | 0x07770000);
//}

//static void nu_pin_emac_init(void)
//{
//    /* EMAC0  */
//    outpw(REG_SYS_GPE_MFPL, 0x11111111);
//    outpw(REG_SYS_GPE_MFPH, (inpw(REG_SYS_GPE_MFPH) & 0xFFFFFF00) | 0x00000011);
//    /* EMAC1  */
//    outpw(REG_SYS_GPF_MFPL, 0x11111111);
//    outpw(REG_SYS_GPF_MFPH, (inpw(REG_SYS_GPF_MFPH) & 0xFFFFFF00) | 0x00000011);
//}

static void nu_pin_qspi_init(void)
{
    /* QSPI0: PD[2, 7]  */
    outpw(REG_SYS_GPD_MFPL, (inpw(REG_SYS_GPD_MFPL) & ~0xFFFFFF00) | 0x11111100);
//    /* QSPI0: PD[2, 5]  */
//    outpw(REG_SYS_GPD_MFPL, (inpw(REG_SYS_GPD_MFPL) & 0xFF0000FF) | 0x00111100);
}

static void nu_pin_spi_init(void)
{
    /* SPI0: PD[8, 11]  */
    outpw(REG_SYS_GPD_MFPH, (inpw(REG_SYS_GPD_MFPH) & ~0x0000FFFF) | 0x00001111);
//    /* SPI0: PC[4, 8]  */
//    outpw(REG_SYS_GPC_MFPL, (inpw(REG_SYS_GPC_MFPL) & 0xF000FFFF) | 0x05560000);
//    outpw(REG_SYS_GPC_MFPH, (inpw(REG_SYS_GPC_MFPH) & 0xFFFFFFF0) | 0x00000005);
}

static void nu_pin_i2c_init(void)
{
    /* I2C1: PB4, PB6 */
    //outpw(REG_SYS_GPB_MFPL, (inpw(REG_SYS_GPB_MFPL) & 0xF0F0FFFF) | 0x02020000);
}

static void nu_pin_can_init(void)
{
    /* CAN3: PA0, PA1 */
    //outpw(REG_SYS_GPA_MFPL, (inpw(REG_SYS_GPA_MFPL) & 0xFFFFFF00) | 0x00000077);
}

static void nu_pin_usbd_init(void)
{
    /* USB0_VBUSVLD, PE.11  */
    outpw(REG_SYS_GPE_MFPH, (inpw(REG_SYS_GPE_MFPH) & 0xFFFF0FFF) | 0x00001000);
}

static void nu_pin_usbh_init(void)
{
}

void nu_pin_init(void)
{
    nu_pin_uart_init();
    nu_pin_emac_init();
    nu_pin_qspi_init();
    nu_pin_spi_init();
    nu_pin_i2c_init();
    nu_pin_can_init();

    nu_pin_usbd_init();
    nu_pin_usbh_init();
}

void nu_pin_deinit(void)
{

}
