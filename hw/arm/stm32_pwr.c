/*
 * STM32 Power control (PWR).
 *
 * Copyright (C) 2020
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "hw/sysbus.h"
#include "hw/arm/stm32.h"
#include "sysemu/sysemu.h"

#define PWR_DEBUG 1

#ifdef PWR_DEBUG
#define pwr_debug(fs,...) \
    fprintf(stderr,"pwr: %s: "fs,__func__,##__VA_ARGS__)
#else
#define pwr_debug(fs,...)
#endif


#define R_PWR_CR 0x00
#define R_PWR_CSR 0x04

/* Device State */
typedef struct Stm32Pwr {
    SysBusDevice busdev;
    MemoryRegion iomem;

    /* Register Values */
    uint32_t PWR_CR;
    uint32_t PWR_CSR;
} Stm32Pwr;


static uint64_t pwr_read(void* arg, hwaddr addr, unsigned size)
{
    Stm32Pwr *s = arg;

    switch(addr) {
    case R_PWR_CR:
        return s->PWR_CR;
    case (R_PWR_CSR):
        return s->PWR_CSR;
    }
    return 0;
}

static void pwr_write(void *arg, hwaddr addr, uint64_t data, unsigned size)
{

    Stm32Pwr *s = arg;

    qemu_system_shutdown_request();

    switch(addr) {
    case R_PWR_CR:
        s->PWR_CR = data & 0xFFFFFFFF;
        break;
    case R_PWR_CSR:
        s->PWR_CSR = data & 0xFFFFFFFF;
        break;
    }

}

static void pwr_reset(DeviceState *dev)
{
    Stm32Pwr *s = STM32_PWR(dev);

    s->PWR_CR = 0;
    s->PWR_CSR = 0;
}

static const MemoryRegionOps pwr_ops = {
    .read = pwr_read,
    .write = pwr_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4
    }
};


static int pwr_init(SysBusDevice *dev) {
    Stm32Pwr *s = STM32_PWR(dev);

    s->PWR_CR = 0;
    s->PWR_CSR = 0;

    memory_region_init_io(&s->iomem, OBJECT(s), &pwr_ops, s, "pwr", 0x3FF);
    sysbus_init_mmio(dev, &s->iomem);

    return 0;
}




static Property pwr_properties[] = {
    /* DEFINE_PROP_PERIPH_T("periph", Stm32Pwr, periph, STM32_PERIPH_UNDEFINED), */
    /* DEFINE_PROP_PTR("stm32_pwr", Stm32Pwr, stm32_ */
    DEFINE_PROP_END_OF_LIST(),
};

static void pwr_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SysBusDeviceClass *sc = SYS_BUS_DEVICE_CLASS(klass);

    sc->init = pwr_init;
    dc->reset = pwr_reset;
    /* dc->vmsd = &vmstate_pwr;  */
    dc->props = pwr_properties;
}


static const TypeInfo pwr_info = {
    .name = "stm32_pwr",
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(Stm32Pwr),
    .class_init = pwr_class_init,
};

static void pwr_register_types(void)
{
    type_register_static(&pwr_info);
}

type_init(pwr_register_types);
