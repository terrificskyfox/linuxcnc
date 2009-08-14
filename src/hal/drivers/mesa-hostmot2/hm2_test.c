
//
//    Copyright (C) 2007-2008 Sebastian Kuzminsky
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//


//
//  This driver behaves like a HostMot2 "low-level I/O" driver, but
//  provides unchanging, compiled-in, information.  It runs without any
//  special hardware plugged in.  Its job is to provide a test-pattern to
//  verify that the hostmot2 driver functions as it ought.
//


#include <linux/pci.h>

#include "rtapi.h"
#include "rtapi_app.h"
#include "rtapi_string.h"

#include "hal.h"

#include "hostmot2.h"
#include "hostmot2-lowlevel.h"
#include "hm2_test.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sebastian Kuzminsky");
MODULE_DESCRIPTION("Test pattern for the hostmot2 driver, does not talk to any hardware");


static char *config[HM2_TEST_MAX_BOARDS];
static int num_config_strings = HM2_TEST_MAX_BOARDS;
module_param_array(config, charp, &num_config_strings, S_IRUGO);
MODULE_PARM_DESC(config, "config string for the AnyIO boards (see hostmot2(9) manpage)");


int test_pattern = 0;
RTAPI_MP_INT(test_pattern, "The test pattern to show to the hostmot2 driver.");


static int comp_id;

static hm2_test_t board[1];




// 
// these are the "low-level I/O" functions exported up
//


static int hm2_test_read(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    hm2_test_t *me = this->private;
    memcpy(buffer, (me->test_pattern + addr), size);
    return 1;  // success
}


static int hm2_test_write(hm2_lowlevel_io_t *this, u32 addr, void *buffer, int size) {
    return 1;  // success
}


static int hm2_test_program_fpga(hm2_lowlevel_io_t *this, const bitfile_t *bitfile) {
    return 0;
}


static int hm2_test_reset(hm2_lowlevel_io_t *this) {
    return 0;
}




int rtapi_app_main(void) {
    hm2_test_t *me;
    hm2_lowlevel_io_t *this;
    int r = 0;

    LL_PRINT("loading HostMot2 test driver with test pattern %d\n", test_pattern);

    comp_id = hal_init(HM2_LLIO_NAME);
    if (comp_id < 0) return comp_id;

    me = &board[0];

    this = &me->llio;
    memset(this, 0, sizeof(hm2_lowlevel_io_t));

    switch (test_pattern) {

        // 
        // this one has nothing
        // 

        case 0: {
            break;
        }


        // 
        // this one has a good IO Cookie, but that's it
        // 

        case 1: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe
            break;
        }


        // 
        // this one has a good IO Cookie and Config Name
        // the idrom offset is 0, and there's nothing there
        // 

        case 2: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe

            me->test_pattern[HM2_ADDR_CONFIGNAME+0] = 'H';
            me->test_pattern[HM2_ADDR_CONFIGNAME+1] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+2] = 'S';
            me->test_pattern[HM2_ADDR_CONFIGNAME+3] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+4] = 'M';
            me->test_pattern[HM2_ADDR_CONFIGNAME+5] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+6] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+7] = '2';

            break;
        }


        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's an invalid IDROM type there
        // 

        case 3: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe

            me->test_pattern[HM2_ADDR_CONFIGNAME+0] = 'H';
            me->test_pattern[HM2_ADDR_CONFIGNAME+1] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+2] = 'S';
            me->test_pattern[HM2_ADDR_CONFIGNAME+3] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+4] = 'M';
            me->test_pattern[HM2_ADDR_CONFIGNAME+5] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+6] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+7] = '2';

            // put the IDROM at 0x400, where it usually lives
            *((u32*)&me->test_pattern[HM2_ADDR_IDROM_OFFSET]) = 0x400;

            // bad idrom type
            *((u32*)&me->test_pattern[0x400]) = 0x1234;

            break;
        }


        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // but the portwidth is 0
        // 

        case 4: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe

            me->test_pattern[HM2_ADDR_CONFIGNAME+0] = 'H';
            me->test_pattern[HM2_ADDR_CONFIGNAME+1] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+2] = 'S';
            me->test_pattern[HM2_ADDR_CONFIGNAME+3] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+4] = 'M';
            me->test_pattern[HM2_ADDR_CONFIGNAME+5] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+6] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+7] = '2';

            // put the IDROM at 0x400, where it usually lives
            *((u32*)&me->test_pattern[HM2_ADDR_IDROM_OFFSET]) = 0x400;

            // standard idrom type
            *((u32*)&me->test_pattern[0x400]) = 2;

            break;
        }


        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // but the portwidth is 29 which is bogus
        // 

        case 5: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe

            me->test_pattern[HM2_ADDR_CONFIGNAME+0] = 'H';
            me->test_pattern[HM2_ADDR_CONFIGNAME+1] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+2] = 'S';
            me->test_pattern[HM2_ADDR_CONFIGNAME+3] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+4] = 'M';
            me->test_pattern[HM2_ADDR_CONFIGNAME+5] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+6] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+7] = '2';

            // put the IDROM at 0x400, where it usually lives
            *((u32*)&me->test_pattern[HM2_ADDR_IDROM_OFFSET]) = 0x400;

            // standard idrom type
            *((u32*)&me->test_pattern[0x400]) = 2;

            // bad PortWidth
            *((u32*)&me->test_pattern[0x424]) = 29;

            break;
        }


        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth
        // 

        case 6: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe

            me->test_pattern[HM2_ADDR_CONFIGNAME+0] = 'H';
            me->test_pattern[HM2_ADDR_CONFIGNAME+1] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+2] = 'S';
            me->test_pattern[HM2_ADDR_CONFIGNAME+3] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+4] = 'M';
            me->test_pattern[HM2_ADDR_CONFIGNAME+5] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+6] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+7] = '2';

            // put the IDROM at 0x400, where it usually lives
            *((u32*)&me->test_pattern[HM2_ADDR_IDROM_OFFSET]) = 0x400;

            // standard idrom type
            *((u32*)&me->test_pattern[0x400]) = 2;

            // good PortWidth
            *((u32*)&me->test_pattern[0x424]) = 24;

            break;
        }


        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, but problematic IOPorts and IOWidth
        // 

        case 7: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe

            me->test_pattern[HM2_ADDR_CONFIGNAME+0] = 'H';
            me->test_pattern[HM2_ADDR_CONFIGNAME+1] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+2] = 'S';
            me->test_pattern[HM2_ADDR_CONFIGNAME+3] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+4] = 'M';
            me->test_pattern[HM2_ADDR_CONFIGNAME+5] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+6] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+7] = '2';

            // put the IDROM at 0x400, where it usually lives
            *((u32*)&me->test_pattern[HM2_ADDR_IDROM_OFFSET]) = 0x400;

            // standard idrom type
            *((u32*)&me->test_pattern[0x400]) = 2;

            // good PortWidth = 24, which is stadard
            *((u32*)&me->test_pattern[0x424]) = 24;

            // IOPorts = 1
            *((u32*)&me->test_pattern[0x41c]) = 1;

            // IOWidth = 99 (!= IOPorts * PortWidth)
            *((u32*)&me->test_pattern[0x420]) = 99;

            break;
        }


        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, but IOPorts doesnt match what the llio said
        // 

        case 8: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe

            me->test_pattern[HM2_ADDR_CONFIGNAME+0] = 'H';
            me->test_pattern[HM2_ADDR_CONFIGNAME+1] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+2] = 'S';
            me->test_pattern[HM2_ADDR_CONFIGNAME+3] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+4] = 'M';
            me->test_pattern[HM2_ADDR_CONFIGNAME+5] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+6] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+7] = '2';

            // put the IDROM at 0x400, where it usually lives
            *((u32*)&me->test_pattern[HM2_ADDR_IDROM_OFFSET]) = 0x400;

            // standard idrom type
            *((u32*)&me->test_pattern[0x400]) = 2;

            // good PortWidth = 24, which is stadard
            *((u32*)&me->test_pattern[0x424]) = 24;

            // IOPorts = 2 (!= what the llio said)
            *((u32*)&me->test_pattern[0x41c]) = 2;

            // IOWidth == IOPorts * PortWidth)
            *((u32*)&me->test_pattern[0x420]) = 48;

            break;
        }


        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, IOPorts, and IOWidth
        // but the clocks are bad
        // 

        case 9: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe

            me->test_pattern[HM2_ADDR_CONFIGNAME+0] = 'H';
            me->test_pattern[HM2_ADDR_CONFIGNAME+1] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+2] = 'S';
            me->test_pattern[HM2_ADDR_CONFIGNAME+3] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+4] = 'M';
            me->test_pattern[HM2_ADDR_CONFIGNAME+5] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+6] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+7] = '2';

            // put the IDROM at 0x400, where it usually lives
            *((u32*)&me->test_pattern[HM2_ADDR_IDROM_OFFSET]) = 0x400;

            // standard idrom type
            *((u32*)&me->test_pattern[0x400]) = 2;

            // IOWidth = (IOPorts * PortWidth)
            *((u32*)&me->test_pattern[0x424]) = 24;
            *((u32*)&me->test_pattern[0x41c]) = 1;
            *((u32*)&me->test_pattern[0x420]) = 24;

            // ClockLow = 12345
            *((u32*)&me->test_pattern[0x428]) = 12345;

            break;
        }


        // 
        // good IO Cookie, Config Name, and IDROM Type
        // the IDROM offset is the usual, 0x400, and there's a good IDROM type there
        // good PortWidth, IOPorts, and IOWidth
        // but the clocks are bad
        // 

        case 10: {
            *((u32*)&me->test_pattern[HM2_ADDR_IOCOOKIE]) = HM2_IOCOOKIE;  // 0x55aacafe

            me->test_pattern[HM2_ADDR_CONFIGNAME+0] = 'H';
            me->test_pattern[HM2_ADDR_CONFIGNAME+1] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+2] = 'S';
            me->test_pattern[HM2_ADDR_CONFIGNAME+3] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+4] = 'M';
            me->test_pattern[HM2_ADDR_CONFIGNAME+5] = 'O';
            me->test_pattern[HM2_ADDR_CONFIGNAME+6] = 'T';
            me->test_pattern[HM2_ADDR_CONFIGNAME+7] = '2';

            // put the IDROM at 0x400, where it usually lives
            *((u32*)&me->test_pattern[HM2_ADDR_IDROM_OFFSET]) = 0x400;

            // standard idrom type
            *((u32*)&me->test_pattern[0x400]) = 2;

            // IOWidth = (IOPorts * PortWidth)
            *((u32*)&me->test_pattern[0x424]) = 24;
            *((u32*)&me->test_pattern[0x41c]) = 1;
            *((u32*)&me->test_pattern[0x420]) = 24;

            // ClockLow = 2e6
            *((u32*)&me->test_pattern[0x428]) = 2e6;

            // ClockHigh = 0
            *((u32*)&me->test_pattern[0x42c]) = 0;

            break;
        }


        default: {
            LL_ERR("unknown test pattern %d", test_pattern); 
            return -ENODEV;
        }
    }


    rtapi_snprintf(me->llio.name, HAL_NAME_LEN, "hm2_test.0");

    me->llio.num_ioport_connectors = 1;
    me->llio.ioport_connector_name[0] = "P99";
    me->llio.fpga_part_number = "none";

    me->llio.program_fpga = hm2_test_program_fpga;
    me->llio.reset = hm2_test_reset;

    me->llio.comp_id = comp_id;
    me->llio.private = me;

    me->llio.threadsafe = 1;

    me->llio.read = hm2_test_read;
    me->llio.write = hm2_test_write;

    r = hm2_register(&board->llio, config[0]);
    if (r != 0) {
        THIS_ERR("hm2_test fails HM2 registration\n");
        return -EIO;
    }

    THIS_PRINT("initialized hm2 test-pattern %d\n", test_pattern);

    hal_ready(comp_id);
    return 0;
}


void rtapi_app_exit(void) {
    hm2_test_t *me = &board[0];

    hm2_unregister(&me->llio);

    LL_PRINT("driver unloaded\n");
    hal_exit(comp_id);
}

