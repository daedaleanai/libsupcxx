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

#include <cstddef>
#include <stdint.h>

namespace io {

const size_t VGA_WIDTH = 80;
const size_t VGA_HEIGHT = 25;

// Hardware text mode color constants.
enum class VgaColor {
  BLACK = 0,
  BLUE = 1,
  GREEN = 2,
  CYAN = 3,
  RED = 4,
  MAGENTA = 5,
  BROWN = 6,
  LIGHT_GREY = 7,
  DARK_GREY = 8,
  LIGHT_BLUE = 9,
  LIGHT_GREEN = 10,
  LIGHT_CYAN = 11,
  LIGHT_RED = 12,
  LIGHT_MAGENTA = 13,
  LIGHT_BROWN = 14,
  WHITE = 15,
};

inline constexpr uint8_t vgaEntryColor(VgaColor fg, VgaColor bg)  {
  return (uint8_t)fg | (uint8_t)bg << 4;
}

inline constexpr uint16_t vgaEntry(uint8_t character, uint8_t color) {
  return (uint16_t)character | (uint16_t)color << 8;
}

inline constexpr size_t vgaIndex(size_t x, size_t y) {
  return y * VGA_WIDTH + x;
}

//void vgaTerminalInitialize();
void vgaTerminalSetColor(uint8_t color);
void vgaTerminalPutEntryAt(char c, uint8_t color, size_t x, size_t y);
void vgaTerminalPutChar(char c);
void vgaTerminalWrite(const char *data, size_t size);
void vgaTerminalWriteString(const char *data);

} // namespace io
