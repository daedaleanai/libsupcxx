//------------------------------------------------------------------------------
// Copyright (C) 2018-2019 Daedalean AG
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

#include "vga.hh"
#include "string.hh"

namespace io {

namespace {

volatile uint16_t *vgaTerminal = (uint16_t*)0xb8000;
size_t vgaTerminalRow = 0;
size_t vgaTerminalColumn = 0;
uint8_t vgaTerminalColor = vgaEntryColor(VgaColor::LIGHT_GREY, VgaColor::BLACK);

struct VgaTerminalInitializer {
  VgaTerminalInitializer() {
    for (size_t y = 0; y < VGA_HEIGHT; ++y) {
      for (size_t x = 0; x < VGA_WIDTH; ++x) {
        vgaTerminal[vgaIndex(x, y)] = vgaEntry(' ', vgaTerminalColor);
      }
    }
  }
} initializer;

} // namespace


void vgaTerminalSetColor(uint8_t color) {
  vgaTerminalColor = color;
}

void vgaTerminalPutEntryAt(char c, uint8_t color, size_t x, size_t y) {
  vgaTerminal[vgaIndex(x, y)] = vgaEntry(c, color);
}

void vgaTerminalPutChar(char c) {
  // newline
  if (c == '\n') {
    vgaTerminalColumn = 0;
    ++vgaTerminalRow;
    if (vgaTerminalRow == VGA_HEIGHT) {
      vgaTerminalRow = 0;
    }
    return;
  }

  // regular character
  vgaTerminalPutEntryAt(c, vgaTerminalColor, vgaTerminalColumn, vgaTerminalRow);
  ++vgaTerminalColumn;
  if (vgaTerminalColumn == VGA_WIDTH) {
    vgaTerminalColumn = 0;
    ++vgaTerminalRow;
    if (vgaTerminalRow == VGA_HEIGHT) {
      vgaTerminalRow = 0;
    }
  }
}

void vgaTerminalWrite(const char *data, size_t size)  {
  for (size_t i = 0; i < size; ++i) {
    vgaTerminalPutChar(data[i]);
  }
}

void vgaTerminalWriteString(const char *data) {
  vgaTerminalWrite(data, strlen(data));
}

} // namespace io
