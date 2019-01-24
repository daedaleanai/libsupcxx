//------------------------------------------------------------------------------
// Copyright (C) 2019 Daedalean AG
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.
//
// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files LICENSE and LICENSE.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#include <cstdint>

// See:
// https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
// https://github.com/dwelch67/raspberrypi/blob/master/uart01/uart01.c
// Although the base seems to be at 0x3f000000 for the BCM2837 on RPi3.
#define AUXENB          (*(volatile uint32_t*)0x3f215004)
#define AUX_MU_CNTL_REG (*(volatile uint32_t*)0x3f215060)
#define AUX_MU_LCR_REG  (*(volatile uint32_t*)0x3f21504c)
#define AUX_MU_IIR_REG  (*(volatile uint32_t*)0x3f215048)
#define AUX_MU_BAUD     (*(volatile uint32_t*)0x3f215068)
#define GPFSEL1         (*(volatile uint32_t*)0x3f200004)
#define GPFSEL2         (*(volatile uint32_t*)0x3f200008)
#define GPPUD           (*(volatile uint32_t*)0x3f200094)
#define GPPUDCLK0       (*(volatile uint32_t*)0x3f200098)
#define AUX_MU_LSR_REG  (*(volatile uint32_t*)0x3f215054)
#define AUX_MU_IO_REG   (*(volatile uint32_t*)0x3f215040)

#define DO_NOTHING(X) {                            \
  for(register uint64_t tmp = 0; tmp < X; ++tmp) { \
    asm volatile("nop");                           \
  }                                                \
}

extern "C" void _initIo() {
  // UART
  AUXENB |= 0x1;          // enable mini UART and any related registers
  AUX_MU_CNTL_REG = 0;    // disable RX and TX, we also don't want any of the
                          // "fancy features"
  AUX_MU_LCR_REG = 0x3;   // 8-bit mode
  AUX_MU_BAUD = 270;      // baud_rate = (system_clock_freq)/(8*(boud_reg+1))
                          // 115200 baud => reg = 270 because clock is 250MHz

  // GPIO
  register uint32_t tmp = GPFSEL1;
  tmp &= ~(7 << 12);      // clear alt selection for gpio14 - we only TX
  tmp |= 2 << 12;         // alt5 for gpio14 - MiniUART
  GPFSEL1 = tmp;

  GPPUD = 0;              // disable pull ups/downs
  DO_NOTHING(150);        // do nothing for a couple of cycles to provide the
                          // required set up time for the control signal
  GPPUDCLK0 = 1 << 14;    // clock the control signal into gpio14
  DO_NOTHING(150);        // do nothing to provide the required hold time for
                          // the control signal
  GPPUDCLK0 = 0;          // remove the clock

  AUX_MU_CNTL_REG = 0x2;  // Enable TX

  // JTAG
  tmp = GPFSEL2;
  tmp &= 0xff00003f;      // clear alternative selection for gpios 22 through 27
  tmp |= 0x006db6c0;      // alternative function 4-JTAG for gpios 22 through 27
  GPFSEL2 = tmp;
  GPPUD = 0;              // disable pull ups/downs
  DO_NOTHING(150);        // do nothing for a couple of cycles to provide the
                          // required set up time for the control signal
  GPPUDCLK0 = 0x0fc00000; // clock the control signal into gpios 22 through 27
  DO_NOTHING(150);        // do nothing to provide the required hold time for
                          // the control signal
  GPPUDCLK0 = 0;          // remove the clock
}

extern "C" void _putChar(char c) {
  // Busy wait for some space in the TX FIFO
  while ((AUX_MU_LSR_REG & 0x20) == 0) {
    DO_NOTHING(10);
  }

  AUX_MU_IO_REG = c;

  // Put the carriage return after a new line
  if (c == '\n') {
    _putChar('\r');
  }
}

extern "C" void abort() {
  // not really abort but will do
  while(true) {
    asm volatile("wfe");
  }
}
