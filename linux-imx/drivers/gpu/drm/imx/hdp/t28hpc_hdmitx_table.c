/******************************************************************************
 *
 * Copyright (C) 2016-2017 Cadence Design Systems, Inc.
 * All rights reserved worldwide.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Copyright 2018 NXP
 *
 ******************************************************************************
 *
 * t28hpc_hdmitx_table.c
 *
 ******************************************************************************
 */

#include "t28hpc_hdmitx_table.h"

/* Table 6. HDMI TX clock control settings (pixel clock is input) */
const u32 t28hpc_hdmitx_clock_control_table_pixel_in[T28HPC_HDMITX_CLOCK_CONTROL_TABLE_ROWS_PIXEL_IN][T28HPC_HDMITX_CLOCK_CONTROL_TABLE_COLS_PIXEL_IN] = {
    { 25000,  42500, 1000,  250000,  425000, 0x05, 0x01, 0x01, 400, 0x182, 0x00A, 2000000, 3400000, 0, 2, 2, 2, 4,  125000,  212500, 0x03,  25000,  42500},
    { 42500,  85000, 1000,  425000,  850000, 0x08, 0x03, 0x01, 320, 0x132, 0x00A, 1700000, 3400000, 0, 1, 1, 2, 4,  212500,  425000, 0x02,  42500,  85000},
    { 85000, 170000, 1000,  850000, 1700000, 0x11, 0x00, 0x07, 340, 0x146, 0x00A, 1700000, 3400000, 0, 1, 1, 2, 2,  425000,  850000, 0x01,  85000, 170000},
    {170000, 340000, 1000, 1700000, 3400000, 0x22, 0x01, 0x07, 340, 0x146, 0x00A, 1700000, 3400000, 0, 1, 1, 2, 1,  850000, 1700000, 0x00, 170000, 340000},
    {340000, 600000, 1000, 3400000, 6000000, 0x3C, 0x03, 0x06, 600, 0x24A, 0x00A, 3400000, 6000000, 1, 1, 1, 2, 1, 1700000, 3000000, 0x00, 340000, 600000},
    { 25000,  34000, 1250,  312000,  425000, 0x04, 0x01, 0x01, 400, 0x182, 0x00A, 2500000, 3400000, 0, 2, 2, 2, 4,  156250,  212500, 0x03,  31250,  42500},
    { 34000,  68000, 1250,  425000,  850000, 0x06, 0x02, 0x01, 300, 0x11E, 0x00A, 1700000, 3400000, 0, 1, 1, 2, 4,  212500,  425000, 0x02,  42500,  85000},
    { 68000, 136000, 1250,  850000, 1700000, 0x0D, 0x02, 0x02, 325, 0x137, 0x00A, 1700000, 3400000, 0, 1, 1, 2, 2,  425000,  850000, 0x01,  85000, 170000},
    {136000, 272000, 1250, 1700000, 3400000, 0x1A, 0x02, 0x04, 325, 0x137, 0x00A, 1700000, 3400000, 0, 1, 1, 2, 1,  850000, 1700000, 0x00, 170000, 340000},
    {272000, 480000, 1250, 3400000, 6000000, 0x30, 0x03, 0x05, 600, 0x24A, 0x00A, 3400000, 6000000, 1, 1, 1, 2, 1, 1700000, 3000000, 0x00, 340000, 600000},
    { 25000,  28000, 1500,  375000,  420000, 0x03, 0x01, 0x01, 360, 0x15A, 0x00A, 3000000, 3360000, 0, 2, 2, 2, 4,  187500,  210000, 0x03,  37500,  42000},
    { 28000,  56000, 1500,  420000,  840000, 0x06, 0x02, 0x01, 360, 0x15A, 0x00A, 1680000, 3360000, 0, 1, 1, 2, 4,  210000,  420000, 0x02,  42000,  84000},
    { 56000, 113000, 1500,  840000, 1695000, 0x0B, 0x00, 0x05, 330, 0x13C, 0x00A, 1680000, 3390000, 0, 1, 1, 2, 2,  420000,  847500, 0x01,  84000, 169500},
    {113000, 226000, 1500, 1695000, 3390000, 0x16, 0x01, 0x05, 330, 0x13C, 0x00A, 1695000, 3390000, 0, 1, 1, 2, 1,  847500, 1695000, 0x00, 169500, 339000},
    {226000, 400000, 1500, 3390000, 6000000, 0x28, 0x03, 0x04, 600, 0x24A, 0x00A, 3390000, 6000000, 1, 1, 1, 2, 1, 1695000, 3000000, 0x00, 339000, 600000},
    { 25000,  42500, 2000,  500000,  850000, 0x05, 0x01, 0x01, 400, 0x182, 0x00A, 2000000, 3400000, 0, 1, 1, 2, 4,  250000,  425000, 0x02,  50000,  85000},
    { 42500,  85000, 2000,  850000, 1700000, 0x08, 0x03, 0x01, 320, 0x132, 0x00A, 1700000, 3400000, 0, 1, 1, 2, 2,  425000,  850000, 0x01,  85000, 170000},
    { 85000, 170000, 2000, 1700000, 3400000, 0x11, 0x00, 0x07, 340, 0x146, 0x00A, 1700000, 3400000, 0, 1, 1, 2, 1,  850000, 1700000, 0x00, 170000, 340000},
    {170000, 300000, 2000, 3400000, 6000000, 0x22, 0x01, 0x06, 680, 0x29A, 0x00A, 3400000, 6000000, 1, 1, 1, 2, 1, 1700000, 3000000, 0x00, 340000, 600000},
    {594000, 594000,  500, 2970000, 2970000, 0x3C, 0x03, 0x06, 600, 0x24A, 0x00A, 5940000, 5940000, 1, 1, 1, 2, 2, 1485000, 1485000, 0x01, 297000, 297000},
    {594000, 594000,  625, 3712000, 3712000, 0x3C, 0x03, 0x06, 375, 0x169, 0x00A, 3712500, 3712500, 1, 1, 1, 2, 1, 1856250, 1856250, 0x00, 371250, 371250},
    {594000, 594000,  750, 4455000, 4455000, 0x3C, 0x03, 0x06, 450, 0x1B4, 0x00A, 4455000, 4455000, 1, 1, 1, 2, 1, 2227500, 2227500, 0x00, 445500, 445500}
};

/* Table 7. HDMI TX PLL tuning settings (pixel clock is input) */
const u32 t28hpc_hdmitx_pll_tuning_table_pixel_in[T28HPC_HDMITX_PLL_TUNING_TABLE_ROWS_PIXEL_IN][T28HPC_HDMITX_PLL_TUNING_TABLE_COLS_PIXEL_IN] = {
    {0, 1700000, 2000000, 0x4, 0x3, 0x0, 0x09, 0x09, 300.0, 0x082},
    {0, 1700000, 2000000, 0x4, 0x3, 0x0, 0x09, 0x09, 320.0, 0x083},
    {0, 1700000, 2000000, 0x4, 0x3, 0x0, 0x09, 0x09, 325.0, 0x083},
    {0, 1700000, 2000000, 0x4, 0x3, 0x0, 0x09, 0x09, 330.0, 0x084},
    {0, 1700000, 2000000, 0x4, 0x3, 0x0, 0x09, 0x09, 340.0, 0x084},
    {0, 1700000, 2000000, 0x4, 0x3, 0x0, 0x09, 0x09, 360.0, 0x086},
    {0, 1700000, 2000000, 0x4, 0x3, 0x0, 0x09, 0x09, 400.0, 0x0A2},
    {1, 2000000, 2400000, 0x4, 0x3, 0x0, 0x09, 0x09, 300.0, 0x047},
    {1, 2000000, 2400000, 0x4, 0x3, 0x0, 0x09, 0x09, 320.0, 0x04B},
    {1, 2000000, 2400000, 0x4, 0x3, 0x0, 0x09, 0x09, 325.0, 0x04C},
    {1, 2000000, 2400000, 0x4, 0x3, 0x0, 0x09, 0x09, 330.0, 0x080},
    {1, 2000000, 2400000, 0x4, 0x3, 0x0, 0x09, 0x09, 340.0, 0x081},
    {1, 2000000, 2400000, 0x4, 0x3, 0x0, 0x09, 0x09, 360.0, 0x082},
    {1, 2000000, 2400000, 0x4, 0x3, 0x0, 0x09, 0x09, 400.0, 0x084},
    {2, 2400000, 2800000, 0x5, 0x3, 0x1, 0x00, 0x07, 300.0, 0x043},
    {2, 2400000, 2800000, 0x5, 0x3, 0x1, 0x00, 0x07, 320.0, 0x045},
    {2, 2400000, 2800000, 0x5, 0x3, 0x1, 0x00, 0x07, 325.0, 0x045},
    {2, 2400000, 2800000, 0x5, 0x3, 0x1, 0x00, 0x07, 330.0, 0x045},
    {2, 2400000, 2800000, 0x5, 0x3, 0x1, 0x00, 0x07, 340.0, 0x086},
    {2, 2400000, 2800000, 0x5, 0x3, 0x1, 0x00, 0x07, 360.0, 0x04A},
    {2, 2400000, 2800000, 0x5, 0x3, 0x1, 0x00, 0x07, 400.0, 0x081},
    {3, 2800000, 3400000, 0x6, 0x3, 0x1, 0x00, 0x07, 300.0, 0x03D},
    {3, 2800000, 3400000, 0x6, 0x3, 0x1, 0x00, 0x07, 320.0, 0x041},
    {3, 2800000, 3400000, 0x6, 0x3, 0x1, 0x00, 0x07, 325.0, 0x041},
    {3, 2800000, 3400000, 0x6, 0x3, 0x1, 0x00, 0x07, 330.0, 0x041},
    {3, 2800000, 3400000, 0x6, 0x3, 0x1, 0x00, 0x07, 340.0, 0x042},
    {3, 2800000, 3400000, 0x6, 0x3, 0x1, 0x00, 0x07, 360.0, 0x043},
    {3, 2800000, 3400000, 0x6, 0x3, 0x1, 0x00, 0x07, 400.0, 0x046},
    {4, 3400000, 3900000, 0x4, 0x3, 0x0, 0x07, 0x0F, 375.0, 0x041},
    {4, 3400000, 3900000, 0x4, 0x3, 0x0, 0x07, 0x0F, 600.0, 0x082},
    {4, 3400000, 3900000, 0x4, 0x3, 0x0, 0x07, 0x0F, 680.0, 0x085},
    {5, 3900000, 4500000, 0x5, 0x3, 0x0, 0x07, 0x0F, 450.0, 0x041},
    {5, 3900000, 4500000, 0x5, 0x3, 0x0, 0x07, 0x0F, 600.0, 0x04B},
    {5, 3900000, 4500000, 0x5, 0x3, 0x0, 0x07, 0x0F, 680.0, 0x082},
    {6, 4500000, 5200000, 0x6, 0x3, 0x1, 0x00, 0x07, 600.0, 0x045},
    {6, 4500000, 5200000, 0x6, 0x3, 0x1, 0x00, 0x07, 680.0, 0x04A},
    {7, 5200000, 6000000, 0x7, 0x3, 0x1, 0x00, 0x07, 600.0, 0x042},
    {7, 5200000, 6000000, 0x7, 0x3, 0x1, 0x00, 0x07, 680.0, 0x045}
};


/* Table 8. HDMI TX clock control settings (pixel clock is output) */
const u32 t28hpc_hdmitx_clock_control_table_pixel_out[T28HPC_HDMITX_CLOCK_CONTROL_TABLE_ROWS_PIXEL_OUT][T28HPC_HDMITX_CLOCK_CONTROL_TABLE_COLS_PIXEL_OUT] = {
    { 27000, 1000,  270000, 0x03, 0x1, 0x1,  240, 0x0BC, 0x030,  80, 0x026, 0x026, 2160000, 0, 2, 2, 2, 4,  135000, 0x3,  27000, 1},
    { 27000, 1250,  337500, 0x03, 0x1, 0x1,  300, 0x0EC, 0x03C, 100, 0x030, 0x030, 2700000, 0, 2, 2, 2, 4,  168750, 0x3,  33750, 1},
    { 27000, 1500,  405000, 0x03, 0x1, 0x1,  360, 0x11C, 0x048, 120, 0x03A, 0x03A, 3240000, 0, 2, 2, 2, 4,  202500, 0x3,  40500, 1},
    { 27000, 2000,  540000, 0x03, 0x1, 0x1,  240, 0x0BC, 0x030,  80, 0x026, 0x026, 2160000, 0, 2, 2, 2, 4,  270000, 0x2,  54000, 1},
    { 54000, 1000,  540000, 0x03, 0x1, 0x1,  480, 0x17C, 0x060,  80, 0x026, 0x026, 4320000, 1, 2, 2, 2, 4,  270000, 0x3,  54000, 1},
    { 54000, 1250,  675000, 0x04, 0x1, 0x1,  400, 0x13C, 0x050,  50, 0x017, 0x017, 2700000, 0, 1, 1, 2, 4,  337500, 0x2,  67500, 1},
    { 54000, 1500,  810000, 0x04, 0x1, 0x1,  480, 0x17C, 0x060,  60, 0x01C, 0x01C, 3240000, 0, 2, 2, 2, 2,  405000, 0x2,  81000, 1},
    { 54000, 2000, 1080000, 0x03, 0x1, 0x1,  240, 0x0BC, 0x030,  40, 0x012, 0x012, 2160000, 0, 2, 2, 2, 1,  540000, 0x1, 108000, 1},
    { 74250, 1000,  742500, 0x03, 0x1, 0x1,  660, 0x20C, 0x084,  80, 0x026, 0x026, 5940000, 1, 2, 2, 2, 4,  371250, 0x3,  74250, 1},
    { 74250, 1250,  928125, 0x04, 0x1, 0x1,  550, 0x1B4, 0x06E,  50, 0x017, 0x017, 3712500, 1, 1, 1, 2, 4,  464062, 0x2,  92812, 1},
    { 74250, 1500, 1113750, 0x04, 0x1, 0x1,  660, 0x20C, 0x084,  60, 0x01C, 0x01C, 4455000, 1, 2, 2, 2, 2,  556875, 0x2, 111375, 1},
    { 74250, 2000, 1485000, 0x03, 0x1, 0x1,  330, 0x104, 0x042,  40, 0x012, 0x012, 2970000, 0, 2, 2, 2, 1,  742500, 0x1, 148500, 1},
    { 99000, 1000,  990000, 0x03, 0x1, 0x1,  440, 0x15C, 0x058,  40, 0x012, 0x012, 3960000, 1, 2, 2, 2, 2,  495000, 0x2,  99000, 1},
    { 99000, 1250, 1237500, 0x03, 0x1, 0x1,  275, 0x0D8, 0x037,  25, 0x00B, 0x00A, 2475000, 0, 1, 1, 2, 2,  618750, 0x1, 123750, 1},
    { 99000, 1500, 1485000, 0x03, 0x1, 0x1,  330, 0x104, 0x042,  30, 0x00D, 0x00D, 2970000, 0, 2, 2, 2, 1,  742500, 0x1, 148500, 1},
    { 99000, 2000, 1980000, 0x03, 0x1, 0x1,  440, 0x15C, 0x058,  40, 0x012, 0x012, 3960000, 1, 2, 2, 2, 1,  990000, 0x1, 198000, 1},
    {148500, 1000, 1485000, 0x03, 0x1, 0x1,  660, 0x20C, 0x084,  40, 0x012, 0x012, 5940000, 1, 2, 2, 2, 2,  742500, 0x2, 148500, 1},
    {148500, 1250, 1856250, 0x04, 0x1, 0x1,  550, 0x1B4, 0x06E,  25, 0x00B, 0x00A, 3712500, 1, 1, 1, 2, 2,  928125, 0x1, 185625, 1},
    {148500, 1500, 2227500, 0x03, 0x1, 0x1,  495, 0x188, 0x063,  30, 0x00D, 0x00D, 4455000, 1, 1, 1, 2, 2, 1113750, 0x1, 222750, 1},
    {148500, 2000, 2970000, 0x03, 0x1, 0x1,  660, 0x20C, 0x084,  40, 0x012, 0x012, 5940000, 1, 2, 2, 2, 1, 1485000, 0x1, 297000, 1},
    {198000, 1000, 1980000, 0x03, 0x1, 0x1,  220, 0x0AC, 0x02C,  10, 0x003, 0x003, 1980000, 0, 1, 1, 2, 1,  990000, 0x0, 198000, 1},
    {198000, 1250, 2475000, 0x03, 0x1, 0x1,  550, 0x1B4, 0x06E,  25, 0x00B, 0x00A, 4950000, 1, 1, 1, 2, 2, 1237500, 0x1, 247500, 1},
    {198000, 1500, 2970000, 0x03, 0x1, 0x1,  330, 0x104, 0x042,  15, 0x006, 0x005, 2970000, 0, 1, 1, 2, 1, 1485000, 0x0, 297000, 1},
    {198000, 2000, 3960000, 0x03, 0x1, 0x1,  440, 0x15C, 0x058,  20, 0x008, 0x008, 3960000, 1, 1, 1, 2, 1, 1980000, 0x0, 396000, 1},
    {297000, 1000, 2970000, 0x03, 0x1, 0x1,  330, 0x104, 0x042,  10, 0x003, 0x003, 2970000, 0, 1, 1, 2, 1, 1485000, 0x0, 297000, 1},
    {297000, 1500, 4455000, 0x03, 0x1, 0x1,  495, 0x188, 0x063,  15, 0x006, 0x005, 4455000, 1, 1, 1, 2, 1, 2227500, 0x0, 445500, 1},
    {297000, 2000, 5940000, 0x03, 0x1, 0x1,  660, 0x20C, 0x084,  20, 0x008, 0x008, 5940000, 1, 1, 1, 2, 1, 2970000, 0x0, 594000, 1},
    {594000, 1000, 5940000, 0x03, 0x1, 0x1,  660, 0x20C, 0x084,  10, 0x003, 0x003, 5940000, 1, 1, 1, 2, 1, 2970000, 0x0, 594000, 1},
    {594000,  750, 4455000, 0x03, 0x1, 0x1,  495, 0x188, 0x063,  10, 0x003, 0x003, 4455000, 1, 1, 1, 2, 1, 2227500, 0x0, 445500, 0},
    {594000,  625, 3712500, 0x04, 0x1, 0x1,  550, 0x1B4, 0x06E,  10, 0x003, 0x003, 3712500, 1, 1, 1, 2, 1, 1856250, 0x0, 371250, 0},
    {594000,  500, 2970000, 0x03, 0x1, 0x1,  660, 0x20C, 0x084,  10, 0x003, 0x003, 5940000, 1, 1, 1, 2, 2, 1485000, 0x1, 297000, 1},
    /* new VESA */
#if 0 /* VESA pixel clock rate support in debuging */
    { 40000, 1000,  400275, 0x05, 0x1, 0x1,  593, 0x126, 0x127,  80, 0x026, 0x026, 3202200, 1, 1, 1, 1, 1,  200137, 0x3,  40027, 1},
    { 40000, 1250,  500175, 0x05, 0x1, 0x1,  741, 0x170, 0x171, 100, 0x030, 0x030, 4001400, 1, 1, 1, 1, 1,  250087, 0x3,  50017, 1},
    { 40000, 1500,  600075, 0x05, 0x1, 0x1,  889, 0x1BA, 0x1BB, 120, 0x03A, 0x03A, 4800600, 1, 1, 1, 1, 1,  300037, 0x3,  60007, 1},
    { 40000, 2000,  800550, 0x05, 0x1, 0x1,  593, 0x126, 0x127,  80, 0x026, 0x026, 3202200, 1, 1, 1, 1, 1,  400275, 0x2,  80055, 1},
    { 65000, 1000,  650025, 0x05, 0x1, 0x1,  963, 0x1DF, 0x1E0,  80, 0x026, 0x026, 5200200, 1, 1, 1, 1, 1,  325012, 0x3,  65002, 1},
    { 65000, 1250,  812700, 0x05, 0x1, 0x1,  602, 0x12B, 0x12B,  50, 0x017, 0x017, 3250800, 1, 1, 1, 1, 1,  406350, 0x2,  81270, 1},
    { 65000, 1500,  974700, 0x05, 0x1, 0x1,  722, 0x167, 0x167,  60, 0x01C, 0x01C, 3898800, 1, 1, 1, 1, 1,  487350, 0x2,  97470, 1},
    { 65000, 2000, 1300050, 0x05, 0x1, 0x1,  963, 0x1DF, 0x1E0,  80, 0x026, 0x026, 5200200, 1, 1, 1, 1, 1,  650025, 0x2, 130005, 1},
    {108000, 1000, 1080000, 0x05, 0x1, 0x1,  800, 0x18E, 0x18E,  40, 0x012, 0x012, 4320000, 1, 1, 1, 1, 1,  540000, 0x2, 108000, 1},
    {108000, 1250, 1350000, 0x05, 0x1, 0x1, 1000, 0x1F2, 0x1F2,  50, 0x017, 0x017, 5400000, 1, 1, 1, 1, 1,  675000, 0x2, 135000, 1},
    {108000, 1500, 1620000, 0x05, 0x1, 0x1,  600, 0x12A, 0x12A,  30, 0x00D, 0x00D, 3240000, 1, 1, 1, 1, 1,  810000, 0x1, 162000, 1},
    {108000, 2000, 2160000, 0x05, 0x1, 0x1,  800, 0x18E, 0x18E,  40, 0x012, 0x012, 4320000, 1, 1, 1, 1, 1, 1080000, 0x1, 216000, 1},
    {154000, 1000, 1539000, 0x05, 0x1, 0x1,  570, 0x11B, 0x11B,  20, 0x008, 0x008, 3078000, 1, 1, 1, 1, 1,  769500, 0x1, 153900, 1},
    {154000, 1250, 1925100, 0x05, 0x1, 0x1,  713, 0x162, 0x163,  25, 0x00A, 0x00B, 3850200, 1, 1, 1, 1, 1,  962550, 0x1, 192510, 1},
    {154000, 1500, 2311200, 0x05, 0x1, 0x1,  856, 0x1AA, 0x1AA,  30, 0x00D, 0x00D, 4622400, 1, 1, 1, 1, 1, 1155600, 0x1, 231120, 1},
    {154000, 2000, 3078000, 0x05, 0x1, 0x1,  570, 0x11B, 0x11B,  20, 0x008, 0x008, 3078000, 1, 1, 1, 1, 1, 1539000, 0x0, 307800, 1}
#endif
};

/* Table 9. HDMI TX PLL tuning settings (pixel clock is output) */
const u32 t28hpc_hdmitx_pll_tuning_table_pixel_out[T28HPC_HDMITX_PLL_TUNING_TABLE_ROWS_PIXEL_OUT][T28HPC_HDMITX_PLL_TUNING_TABLE_COLS_PIXEL_OUT] = {
    { 1, 1980000, 1980000, 0x4, 0x3, 0x0, 0x09, 0x09,  220, 0x42, 160, 5, 183},
    { 2, 2160000, 2160000, 0x4, 0x3, 0x0, 0x09, 0x09,  240, 0x42, 166, 6, 208},
    { 3, 2475000, 2475000, 0x5, 0x3, 0x1, 0x00, 0x07,  275, 0x42, 167, 6, 209},
    { 4, 2700000, 2700000, 0x5, 0x3, 0x1, 0x00, 0x07,  300, 0x42, 188, 6, 230},
    { 4, 2700000, 2700000, 0x5, 0x3, 0x1, 0x00, 0x07,  400, 0x4C, 188, 6, 230},
    { 5, 2970000, 2970000, 0x6, 0x3, 0x1, 0x00, 0x07,  330, 0x42, 183, 6, 225},
    { 6, 3240000, 3240000, 0x6, 0x3, 0x1, 0x00, 0x07,  360, 0x42, 203, 7, 256},
    { 6, 3240000, 3240000, 0x6, 0x3, 0x1, 0x00, 0x07,  480, 0x4C, 203, 7, 256},
    { 7, 3712500, 3712500, 0x4, 0x3, 0x0, 0x07, 0x0F,  550, 0x4C, 212, 7, 257},
    { 8, 3960000, 3960000, 0x5, 0x3, 0x0, 0x07, 0x0F,  440, 0x42, 184, 6, 226},
    { 9, 4320000, 4320000, 0x5, 0x3, 0x1, 0x07, 0x0F,  480, 0x42, 205, 7, 258},
    {10, 4455000, 4455000, 0x5, 0x3, 0x0, 0x07, 0x0F,  495, 0x42, 219, 7, 272},
    {10, 4455000, 4455000, 0x5, 0x3, 0x0, 0x07, 0x0F,  660, 0x4C, 219, 7, 272},
    {11, 4950000, 4950000, 0x6, 0x3, 0x1, 0x00, 0x07,  550, 0x42, 213, 7, 258},
    {12, 5940000, 5940000, 0x7, 0x3, 0x1, 0x00, 0x07,  660, 0x42, 244, 8, 292},
    /* new VESA */
    {13, 3078000, 3078000, 0x6, 0x3, 0x1, 0x00, 0x07,  570, 0x4C, 183, 6, 225}, /* nominal VCO freq: 3078000 */
    {14, 3202100, 3202300, 0x6, 0x3, 0x1, 0x00, 0x07,  593, 0x4C, 203, 7, 256}, /* nominal VCO freq: 3202200 */
    {15, 3240000, 3240000, 0x6, 0x3, 0x1, 0x00, 0x07,  600, 0x4C, 203, 7, 256}, /* nominal VCO freq: 3240000 */
    {16, 3250700, 3250900, 0x6, 0x3, 0x1, 0x00, 0x07,  602, 0x4C, 203, 7, 256}, /* nominal VCO freq: 3250800 */
    {17, 3850100, 3850300, 0x5, 0x3, 0x0, 0x07, 0x0F,  713, 0x42, 184, 6, 226}, /* nominal VCO freq: 3850200 */
    {18, 3898700, 3898900, 0x5, 0x3, 0x0, 0x07, 0x0F,  722, 0x42, 184, 6, 226}, /* nominal VCO freq: 3898800 */
    {19, 4001300, 4001500, 0x5, 0x3, 0x0, 0x07, 0x0F,  741, 0x42, 184, 6, 226}, /* nominal VCO freq: 4001400 */
    {20, 4320000, 4320000, 0x5, 0x3, 0x1, 0x07, 0x0F,  800, 0x42, 205, 7, 258}, /* nominal VCO freq: 4320000 */
    {21, 4622300, 4622500, 0x5, 0x3, 0x0, 0x07, 0x0F,  856, 0x4C, 219, 7, 272}, /* nominal VCO freq: 4622400 */
    {22, 4800500, 4800700, 0x6, 0x3, 0x1, 0x00, 0x07,  889, 0x42, 213, 7, 258}, /* nominal VCO freq: 4800600 */
    {23, 5200100, 5200300, 0x6, 0x3, 0x1, 0x00, 0x07,  963, 0x42, 213, 7, 258}, /* nominal VCO freq: 5200200 */
    {24, 5400000, 5400000, 0x6, 0x3, 0x1, 0x00, 0x07, 1000, 0x42, 213, 7, 258}  /* nominal VCO freq: 5400000 */
};