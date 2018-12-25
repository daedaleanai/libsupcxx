
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
