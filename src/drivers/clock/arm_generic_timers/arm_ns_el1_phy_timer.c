/**
 * @file
 *
 * @date Nov 24, 2020
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>

#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <framework/mod/options.h>

#define ARM_NS_EL1_PHY_TIMER_IRQN  29
//OPTION_GET(NUMBER, irq_num)

#define CNTP_CTL_ENABLE  (1 << 0)
#define CNTP_CTL_MASK    (1 << 1)
#define CNTP_CTL_STATUS  (1 << 2)
#include <kernel/printk.h>
#define PBASE 0x20000000
#define ENABLE_IRQS_1	(PBASE+0x0000B210)

static inline uint32_t arch_timer_get_cntfrq(void) {
	uint32_t val;
	//asm volatile("mrs %0,   cntfrq_el0" : "=r" (val));
	asm volatile("mrc p15, 0, %0, c14, c0, 0" : "=r" (val) : : "cc");
	return val;
}

static inline uint32_t el1_phy_timer_get_ctrl(void) {
	uint32_t val;
	//MRC p15, 0, <Rt>, c14, c3, 1      ; Read CNTV_CTL into Rt
	asm volatile("mrc p15, 0, %0, c14, c3, 1" : "=r" (val) : : "cc");
	return val;
}

static inline void el1_phy_timer_set_ctrl(uint32_t val) {
	// MMCR p15, 0, <Rt>, c14, c3, 1      ; Write Rt to CNTV_CTL
	asm volatile("mcr p15, 0, %0, c14, c3, 1" : : "r" (val));
}
/*
static inline void el1_phy_timer_set_tval(uint32_t val) {
	// MCR p15, 0, <Rt>, c14, c3, 0      ; Write Rt to CNTV_TVAL 
	asm volatile("mcr p15, 0, %0, c14, c3, 0" : : "r" (val));
}
*/

static inline uint64_t arch_timer_get_cntv_cval(void) {
	uint64_t val;
	//MRRC p15, 2, <Rt>, <Rt2>, c14; Read 64-bit CNTP_CVAL into Rt (low word) and Rt2 (high word)
	asm volatile("mrrc p15, 3, %Q0, %R0, c14" : "=r" (val) : : "cc");
	return val;
}

static void arch_timer_set_cntv_cval(uint64_t val) {
	//MCRR p15, 3, <Rt>, <Rt2>, c14  
	asm volatile("mcrr p15, 3, %Q0, %R0, c14" : "=r" (val) : : "cc");
}

/*
static inline uint64_t arch_timer_get_cntvct(void) {
	uint64_t val;
	//MRRC p15, 1, <Rt>, <Rt2>, c14     ; Read 64-bit CNTVCT into Rt (low word) and Rt2 (high word)
	//значение счетчика виртуального таймера (count в qemu)
	asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (val) : : "cc");
	return val;
}

static inline uint32_t el1_phy_timer_get_tval(void) {
	uint32_t val;
	//asm volatile("mrs %0, cntp_tval_el0" : "=r" (val));
	// MRC p15, 0, <Rt>, c14, c2, 0      ; Read CNTP_TVAL into Rt 
	asm volatile("mrc p15, 0, %0, c14, c2, 0" : "=r" (val) : : "cc");
	return val;
}

static void arch_timer_set_cntp_cval(uint64_t val) { 
	//MCRR p15, 2, <Rt>, <Rt2>, c14       ; Write Rt (low word) and Rt2 (high word) to 64-bit CNTP_CVAL
	asm volatile("mcrr p15, 2, %Q0, %R0, c14" : : "r" (val));
}

static inline void el1_phy_timer_set_tval(uint32_t val) {
	//asm volatile("msr cntp_tval_el0, %0" : : "r" (val));
	// MCR p15, 0, <Rt>, c14, c2, 0      ; Write Rt to CNTP_TVAL 
	asm volatile("mcr p15, 0, %0, c14, c2, 0" : : "r" (val));
}

static inline void el1_phy_timer_set_ctrl(uint32_t val) {
	//asm volatile("msr cntp_ctl_el0, %0" : : "r" (val));
	// MCR p15, 0, <Rt>, c14, c2, 1      ; Write Rt to CNTP_CTL
	asm volatile("mcr p15, 0, %0, c14, c2, 1" : : "r" (val));
}

static inline uint32_t el1_phy_timer_get_ctrl(void) {
	uint32_t val;
	//asm volatile("mrs cntp_ctl_el0, %0" : : "r" (val));
	//MRC p15, 0, <Rt>, c14, c2, 1      ; Read CNTP_CTL into Rt
	asm volatile("mrc p15, 0, %0, c14, c2, 1" : "=r" (val) : : "cc");
	return val;
}

static inline uint64_t arch_timer_get_cntp_cval(void) {
	uint64_t val;
	//MRRC p15, 2, <Rt>, <Rt2>, c14       ; Read 64-bit CNTP_CVAL into Rt (low word) and Rt2 (high word)
	asm volatile("mrrc p15, 2, %Q0, %R0, c14" : "=r" (val) : : "cc");
	return val;
}

static inline uint32_t arch_timer_get_cntkctl(void) {
	uint32_t val;
	//MRC p15, 0, <Rt>, c14, c1, 0    ; Read CNTKCTL to Rt
	asm volatile("mrc p15, 0, %0, c14, c1, 0" : "=r" (val) : : "cc");
	return val;
}

static void arch_timer_set_cntkctl(uint32_t val) {
	//MCR p15, 0, <Rt>, c14, c1, 0    ; Write Rt to CNTKCTL
	asm volatile("mcr p15, 0, %0, c14, c1, 0" : : "r" (val));
}
*/

static irq_return_t el1_phy_timer_handler(unsigned int irq_nr, void *dev_id) {
	uint32_t freq;
	printk("handled");
	freq = arch_timer_get_cntfrq();
--freq;
	//el1_phy_timer_set_tval(freq / 1000);
	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

static int el1_phy_timer_set_periodic(struct clock_source *cs) {
	uint32_t freq;
	freq = arch_timer_get_cntfrq();
	--freq; 
		
		
	uint32_t val = el1_phy_timer_get_ctrl();
	printk("ctrl%u", val);
	el1_phy_timer_set_ctrl(CNTP_CTL_ENABLE);
	val = el1_phy_timer_get_ctrl();
	printk("ctrl%u", val);  
	

	uint64_t val1 = arch_timer_get_cntv_cval();
	printk("cval%llu", val1);
	arch_timer_set_cntv_cval(100000);
	val1 = arch_timer_get_cntv_cval();
	printk("cval%llu", val1);
	
	
	
	val = el1_phy_timer_get_ctrl();
	printk("ctrl%uR", val);
	return ENOERR;
}

static struct time_event_device el1_phy_timer_event_device  = {
	.set_periodic = el1_phy_timer_set_periodic,
	.name = "el1_phy_timer",
	.irq_nr = ARM_NS_EL1_PHY_TIMER_IRQN
};

static int el1_phy_timer_init(struct clock_source *cs) {
	printk("inited");
	return irq_attach(ARM_NS_EL1_PHY_TIMER_IRQN, el1_phy_timer_handler, 0, cs, "ARM NS EL1 generic timer");
}

CLOCK_SOURCE_DEF(arm_el1_phy_timer, el1_phy_timer_init, NULL,
	&el1_phy_timer_event_device, NULL);
