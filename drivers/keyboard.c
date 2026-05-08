#include "keyboard.h"
#include "../kernel/arch/x86/io.h"
#include "../lib/vga.h"
#include <stdbool.h>

static volatile uint8_t kbd_buf[256];
static volatile uint8_t kbd_head = 0;
static volatile uint8_t kbd_tail = 0;

static bool shift_held = false;
static bool ctrl_held = false;
static bool alt_held = false;
static bool caps_active = false;
static bool extended = false;

/* indexed by scancode (0x00 - 0x58), 0 = non-printable / modifier */
const char scancode_ascii[0x59] = {
    /*00*/ 0, /* UNKNOWN               */
    /*01*/ 0, /* ESC                   */
    /*02*/ '1',  '2',  '3', '4', '5', '6', '7', '8', '9',
    '0',
    /*0C*/ '-',  '=',
    /*0E*/ '\b', /* BACKSPACE             */
    /*0F*/ '\t', /* TAB                   */
    /*10*/ 'q',  'w',  'e', 'r', 't', 'y', 'u', 'i', 'o',
    'p',
    /*1A*/ '[',  ']',
    /*1C*/ '\n', /* ENTER                 */
    /*1D*/ 0,    /* LCTRL                 */
    /*1E*/ 'a',  's',  'd', 'f', 'g', 'h', 'j', 'k', 'l',
    /*27*/ ';',  '\'', '`',
    /*2A*/ 0, /* LSHIFT                */
    /*2B*/ '\\',
    /*2C*/ 'z',  'x',  'c', 'v', 'b', 'n', 'm',
    /*33*/ ',',  '.',  '/',
    /*36*/ 0,   /* RSHIFT                */
    /*37*/ '*', /* KP *                  */
    /*38*/ 0,   /* LALT                  */
    /*39*/ ' ', /* SPACE                 */
    /*3A*/ 0,   /* CAPSLOCK              */
    /*3B*/ 0,    0,    0,   0,   0,   0,   0,   0,   0,
    0,                      /* F1-F10        */
    /*45*/ 0,               /* NUMLOCK               */
    /*46*/ 0,               /* SCROLLLOCK            */
    /*47*/ '7',  '8',  '9', /* KP 7-9                */
    /*4A*/ '-',             /* KP -                  */
    /*4B*/ '4',  '5',  '6', /* KP 4-6                */
    /*4E*/ '+',             /* KP +                  */
    /*4F*/ '1',  '2',  '3', /* KP 1-3                */
    /*52*/ '0',             /* KP 0                  */
    /*53*/ '.',             /* KP .                  */
    /*54*/ 0,    0,    0,   /* gap (0x54-0x56)       */
    /*57*/ 0,    0,         /* F11, F12              */
};

/* shifted variant — same layout */
const char scancode_ascii_shift[0x59] = {
    /*00*/ 0,
    /*01*/ 0, /* ESC                   */
    /*02*/ '!',  '@', '#', '$', '%', '^', '&', '*', '(',
    ')',
    /*0C*/ '_',  '+',
    /*0E*/ '\b',
    /*0F*/ '\t',
    /*10*/ 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',
    'P',
    /*1A*/ '{',  '}',
    /*1C*/ '\n',
    /*1D*/ 0, /* LCTRL                 */
    /*1E*/ 'A',  'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    /*27*/ ':',  '"', '~',
    /*2A*/ 0, /* LSHIFT                */
    /*2B*/ '|',
    /*2C*/ 'Z',  'X', 'C', 'V', 'B', 'N', 'M',
    /*33*/ '<',  '>', '?',
    /*36*/ 0, /* RSHIFT                */
    /*37*/ '*',
    /*38*/ 0, /* LALT                  */
    /*39*/ ' ',
    /*3A*/ 0, /* CAPSLOCK              */
    /*3B*/ 0,    0,   0,   0,   0,   0,   0,   0,   0,
    0, /* F1-F10        */
    /*45*/ 0,
    /*46*/ 0,
    /*47*/ '7',  '8', '9',
    /*4A*/ '-',
    /*4B*/ '4',  '5', '6',
    /*4E*/ '+',
    /*4F*/ '1',  '2', '3',
    /*52*/ '0',
    /*53*/ '.',
    /*54*/ 0,    0,   0,
    /*57*/ 0,    0,
};

/* extended keys (0xE0 prefix), indexed by low byte (0x00 - 0x6D)
   navigation keys return 0 — handle them separately via the KEYCODE enum */
const char scancode_ext_ascii[0x6E] = {
    /*00*/ 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,           0, 0,                            /* 0x00-0x0F              */
    /*10*/ 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x10-0x1B              */
    /*1C*/ '\n', /* KP ENTER                            */
    /*1D*/ 0,    /* RCTRL                               */
    /*1E*/ 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,           0, 0,             /* 0x1E-0x2D              */
    /*2E*/ 0,                      /* VOL DOWN (multimedia)               */
    /*2F*/ 0,                      /* 0x2F                   */
    /*30*/ 0,                      /* VOL UP   (multimedia)               */
    /*31*/ 0,                      /* 0x31                                */
    /*32*/ 0,                      /* WWW HOME (multimedia)               */
    /*33*/ 0,    0,                /* 0x33-0x34                           */
    /*35*/ '/',                    /* KP /                                */
    /*36*/ 0,    0,                /* 0x36-0x37                           */
    /*38*/ 0,                      /* RALT                                */
    /*39*/ 0,    0, 0, 0, 0, 0, 0, /* 0x39-0x3F                           */
    /*40*/ 0,    0, 0, 0, 0, 0, 0, /* 0x40-0x46                           */
    /*47*/ 0,                      /* HOME   (no ASCII)                   */
    /*48*/ 0,                      /* UP                                  */
    /*49*/ 0,                      /* PAGE UP                             */
    /*4A*/ 0,                      /* 0x4A                                */
    /*4B*/ 0,                      /* LEFT                                */
    /*4C*/ 0,                      /* 0x4C                                */
    /*4D*/ 0,                      /* RIGHT                               */
    /*4E*/ 0,                      /* 0x4E                                */
    /*4F*/ 0,                      /* END                                 */
    /*50*/ 0,                      /* DOWN                                */
    /*51*/ 0,                      /* PAGE DOWN                           */
    /*52*/ 0,                      /* INSERT                              */
    /*53*/ 0,                      /* DELETE                              */
    /*54*/ 0,    0, 0, 0, 0, 0, 0, /* 0x54-0x5A                           */
    /*5B*/ 0,                      /* LGUI                                */
    /*5C*/ 0,                      /* RGUI                                */
    /*5D*/ 0,                      /* APPS                                */
    /*5E*/ 0,                      /* ACPI POWER                          */
    /*5F*/ 0,                      /* ACPI SLEEP                          */
    /*60*/ 0,    0, 0,             /* 0x60-0x62                           */
    /*63*/ 0,                      /* ACPI WAKE                           */
    /*64*/ 0,                      /* 0x64                                */
    /*65*/ 0,    0, 0, 0, 0, 0, 0, 0, 0, /* WWW keys (multimedia) 0x65-0x6D */
};

static const char scancode_to_ascii(uint8_t sc, bool extended, bool upper) {
  if (extended) {
    if (sc >= 0x6E)
      return 0;
    return scancode_ext_ascii[sc]; /* shift doesn't affect ext keys */
  }
  if (sc >= 0x59)
    return 0;
  return upper ? scancode_ascii_shift[sc] : scancode_ascii[sc];
}

const char keyboard_poll(void) {
  if (kbd_head == kbd_tail)
    return 0;
  uint8_t scancode = kbd_buf[kbd_tail];
  kbd_tail = (kbd_tail + 1) % 256;

  if (scancode == 0xE0) {
    extended = true;
    return 0;
  }

  bool released = scancode & 0x80;
  enum KEYCODE key = scancode & 0x7F;

  // modifier handling — must run for both press AND release
  if (key == KEY_LSHIFT || key == KEY_RSHIFT) {
    shift_held = !released;
    return 0;
  }
  if (key == KEY_LCTRL || key == KEY_RCTRL) {
    ctrl_held = !released;
    return 0;
  }
  if (key == KEY_LALT || key == KEY_RALT) {
    alt_held = !released;
    return 0;
  }
  if (key == KEY_CAPSLOCK && !released) {
    caps_active = !caps_active;
    return 0;
  }
  // non-modifier release: ignore
  if (released) {
    return 0;
  }
  if (key == KEY_BACKSPACE) {
    if (get_cols() > 0) {
      decrement_cols();
      volatile char *cell = get_cell();
      cell[0] = ' ';
    } else if (get_cols() == 0) {
      decrement_rows();
    }
    return 0;
  }
  if (key == KEY_ENTER) {
    print_nl();
  }
  bool upper = shift_held ^ caps_active; // XOR: caps inverts shift
  char ascii = scancode_to_ascii(key, extended, upper);
  extended = false;
  return ascii;
}

void keyboard_irq(void) {
  uint8_t scancode = inb(PS2_DATA);
  kbd_buf[kbd_head] = scancode;
  kbd_head = (kbd_head + 1);
}
