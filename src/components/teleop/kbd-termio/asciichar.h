/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ASCII_CHARACTERS_H
#define ORCA2_ASCII_CHARACTERS_H

// lower CASE
#define KEYCODE_a 0x61
#define KEYCODE_b 0x62
#define KEYCODE_c 0x63
#define KEYCODE_d 0x64
#define KEYCODE_e 0x65
#define KEYCODE_f 0x66
#define KEYCODE_g 0x67
#define KEYCODE_h 0x68
#define KEYCODE_i 0x69
#define KEYCODE_j 0x6a
#define KEYCODE_k 0x6b
#define KEYCODE_l 0x6c
#define KEYCODE_m 0x6d
#define KEYCODE_n 0x6e
#define KEYCODE_o 0x6f
#define KEYCODE_p 0x70
#define KEYCODE_q 0x71
#define KEYCODE_r 0x72
#define KEYCODE_s 0x73
#define KEYCODE_t 0x74
#define KEYCODE_u 0x75
#define KEYCODE_v 0x76
#define KEYCODE_w 0x77
#define KEYCODE_x 0x78
#define KEYCODE_y 0x79
#define KEYCODE_z 0x7a
// UPPER CASE
#define KEYCODE_A 0x41
#define KEYCODE_B 0x42
#define KEYCODE_C 0x43
#define KEYCODE_D 0x44
#define KEYCODE_E 0x45
#define KEYCODE_F 0x46
#define KEYCODE_G 0x47
#define KEYCODE_H 0x48
#define KEYCODE_I 0x49
#define KEYCODE_J 0x4a
#define KEYCODE_K 0x4b
#define KEYCODE_L 0x4c
#define KEYCODE_M 0x4d
#define KEYCODE_N 0x4e
#define KEYCODE_O 0x4f
#define KEYCODE_P 0x50
#define KEYCODE_Q 0x51
#define KEYCODE_R 0x52
#define KEYCODE_S 0x53
#define KEYCODE_T 0x54
#define KEYCODE_U 0x55
#define KEYCODE_V 0x56
#define KEYCODE_W 0x57
#define KEYCODE_X 0x58
#define KEYCODE_Y 0x59
#define KEYCODE_Z 0x5a
// other
#define KEYCODE_ESCAPE  0x1B

/*
enum EKEY_CODE
{
KEY_LBUTTON          = 0x01,  // Left mouse button
KEY_RBUTTON          = 0x02,  // Right mouse button
KEY_CANCEL           = 0x03,  // Control-break processing
KEY_MBUTTON          = 0x04,  // Middle mouse button (three-button mouse)
KEY_XBUTTON1         = 0x05,  // Windows 2000/XP: X1 mouse button
KEY_XBUTTON2         = 0x06,  // Windows 2000/XP: X2 mouse button
KEY_BACK             = 0x08,  // BACKSPACE key
KEY_TAB              = 0x09,  // TAB key
KEY_CLEAR            = 0x0C,  // CLEAR key
KEY_RETURN           = 0x0D,  // ENTER key
KEY_SHIFT            = 0x10,  // SHIFT key
KEY_CONTROL          = 0x11,  // CTRL key
KEY_MENU             = 0x12,  // ALT key
KEY_PAUSE            = 0x13,  // PAUSE key
KEY_CAPITAL          = 0x14,  // CAPS LOCK key
KEY_KANA             = 0x15,  // IME Kana mode
KEY_HANGUEL          = 0x15,  // IME Hanguel mode (maintained for compatibility use KEY_HANGUL)
KEY_HANGUL           = 0x15,  // IME Hangul mode
KEY_JUNJA            = 0x17,  // IME Junja mode
KEY_FINAL            = 0x18,  // IME final mode
KEY_HANJA            = 0x19,  // IME Hanja mode
KEY_KANJI            = 0x19,  // IME Kanji mode
KEY_ESCAPE           = 0x1B,  // ESC key
KEY_CONVERT          = 0x1C,  // IME convert
KEY_NONCONVERT       = 0x1D,  // IME nonconvert
KEY_ACCEPT           = 0x1E,  // IME accept
KEY_MODECHANGE       = 0x1F,  // IME mode change request
KEY_SPACE            = 0x20,  // SPACEBAR
KEY_PRIOR            = 0x21,  // PAGE UP key
KEY_NEXT             = 0x22,  // PAGE DOWN key
KEY_END              = 0x23,  // END key
KEY_HOME             = 0x24,  // HOME key
KEY_LEFT             = 0x25,  // LEFT ARROW key
KEY_UP               = 0x26,  // UP ARROW key
KEY_RIGHT            = 0x27,  // RIGHT ARROW key
KEY_DOWN             = 0x28,  // DOWN ARROW key
KEY_SELECT           = 0x29,  // SELECT key
KEY_PRINT            = 0x2A,  // PRINT key
KEY_EXECUT           = 0x2B,  // EXECUTE key
KEY_SNAPSHOT         = 0x2C,  // PRINT SCREEN key
KEY_INSERT           = 0x2D,  // INS key
KEY_DELETE           = 0x2E,  // DEL key
KEY_HELP             = 0x2F,  // HELP key
KEY_KEY_0            = 0x30,  // 0 key
KEY_KEY_1            = 0x31,  // 1 key
KEY_KEY_2            = 0x32,  // 2 key
KEY_KEY_3            = 0x33,  // 3 key
KEY_KEY_4            = 0x34,  // 4 key
KEY_KEY_5            = 0x35,  // 5 key
KEY_KEY_6            = 0x36,  // 6 key
KEY_KEY_7            = 0x37,  // 7 key
KEY_KEY_8            = 0x38,  // 8 key
KEY_KEY_9            = 0x39,  // 9 key
KEY_KEY_A            = 0x41,  // A key
KEY_KEY_B            = 0x42,  // B key
KEY_KEY_C            = 0x43,  // C key
KEY_KEY_D            = 0x44,  // D key
KEY_KEY_E            = 0x45,  // E key
KEY_KEY_F            = 0x46,  // F key
00072                 KEY_KEY_G            = 0x47,  // G key
00073                 KEY_KEY_H            = 0x48,  // H key
00074                 KEY_KEY_I            = 0x49,  // I key
00075                 KEY_KEY_J            = 0x4A,  // J key
00076                 KEY_KEY_K            = 0x4B,  // K key
00077                 KEY_KEY_L            = 0x4C,  // L key
00078                 KEY_KEY_M            = 0x4D,  // M key
00079                 KEY_KEY_N            = 0x4E,  // N key
00080                 KEY_KEY_O            = 0x4F,  // O key
00081                 KEY_KEY_P            = 0x50,  // P key
00082                 KEY_KEY_Q            = 0x51,  // Q key
00083                 KEY_KEY_R            = 0x52,  // R key
00084                 KEY_KEY_S            = 0x53,  // S key
00085                 KEY_KEY_T            = 0x54,  // T key
00086                 KEY_KEY_U            = 0x55,  // U key
00087                 KEY_KEY_V            = 0x56,  // V key
00088                 KEY_KEY_W            = 0x57,  // W key
00089                 KEY_KEY_X            = 0x58,  // X key
00090                 KEY_KEY_Y            = 0x59,  // Y key
00091                 KEY_KEY_Z            = 0x5A,  // Z key
00092                 KEY_LWIN             = 0x5B,  // Left Windows key (Microsoft® Natural® keyboard)
00093                 KEY_RWIN             = 0x5C,  // Right Windows key (Natural keyboard)
00094                 KEY_APPS             = 0x5D,  //Applications key (Natural keyboard)
00095                 KEY_SLEEP            = 0x5F,  // Computer Sleep key
00096                 KEY_NUMPAD0          = 0x60,  // Numeric keypad 0 key
00097                 KEY_NUMPAD1          = 0x61,  // Numeric keypad 1 key
00098                 KEY_NUMPAD2          = 0x62,  // Numeric keypad 2 key
00099                 KEY_NUMPAD3          = 0x63,  // Numeric keypad 3 key
00100                 KEY_NUMPAD4          = 0x64,  // Numeric keypad 4 key
00101                 KEY_NUMPAD5          = 0x65,  // Numeric keypad 5 key
00102                 KEY_NUMPAD6          = 0x66,  // Numeric keypad 6 key
00103                 KEY_NUMPAD7          = 0x67,  // Numeric keypad 7 key
00104                 KEY_NUMPAD8          = 0x68,  // Numeric keypad 8 key
00105                 KEY_NUMPAD9          = 0x69,  // Numeric keypad 9 key
00106                 KEY_MULTIPLY         = 0x6A,  // Multiply key
00107                 KEY_ADD              = 0x6B,  // Add key
00108                 KEY_SEPARATOR        = 0x6C,  // Separator key
00109                 KEY_SUBTRACT         = 0x6D,  // Subtract key
00110                 KEY_DECIMAL          = 0x6E,  // Decimal key
00111                 KEY_DIVIDE           = 0x6F,  // Divide key
00112                 KEY_F1               = 0x70,  // F1 key
00113                 KEY_F2               = 0x71,  // F2 key
00114                 KEY_F3               = 0x72,  // F3 key
00115                 KEY_F4               = 0x73,  // F4 key
00116                 KEY_F5               = 0x74,  // F5 key
00117                 KEY_F6               = 0x75,  // F6 key
00118                 KEY_F7               = 0x76,  // F7 key
00119                 KEY_F8               = 0x77,  // F8 key
00120                 KEY_F9               = 0x78,  // F9 key
00121                 KEY_F10              = 0x79,  // F10 key
00122                 KEY_F11              = 0x7A,  // F11 key
00123                 KEY_F12              = 0x7B,  // F12 key
00124                 KEY_F13              = 0x7C,  // F13 key
00125                 KEY_F14              = 0x7D,  // F14 key
00126                 KEY_F15              = 0x7E,  // F15 key
00127                 KEY_F16              = 0x7F,  // F16 key
00128                 KEY_F17              = 0x80,  // F17 key
00129                 KEY_F18              = 0x81,  // F18 key
00130                 KEY_F19              = 0x82,  // F19 key
00131                 KEY_F20              = 0x83,  // F20 key
00132                 KEY_F21              = 0x84,  // F21 key
00133                 KEY_F22              = 0x85,  // F22 key
00134                 KEY_F23              = 0x86,  // F23 key
00135                 KEY_F24              = 0x87,  // F24 key
00136                 KEY_NUMLOCK          = 0x90,  // NUM LOCK key
00137                 KEY_SCROLL           = 0x91,  // SCROLL LOCK key
00138                 KEY_LSHIFT           = 0xA0,  // Left SHIFT key
00139                 KEY_RSHIFT           = 0xA1,  // Right SHIFT key
00140                 KEY_LCONTROL         = 0xA2,  // Left CONTROL key
00141                 KEY_RCONTROL         = 0xA3,  // Right CONTROL key
00142                 KEY_LMENU            = 0xA4,  // Left MENU key
00143                 KEY_RMENU            = 0xA5,  // Right MENU key
00144                 KEY_COMMA            = 0xBC,  // Comma Key  (,)
00145                 KEY_PLUS             = 0xBB,  // Plus Key   (+)
00146                 KEY_MINUS            = 0xBD,  // Minus Key  (-)
00147                 KEY_PERIOD           = 0xBE,  // Period Key (.)
00148                 KEY_ATTN             = 0xF6,  // Attn key
00149                 KEY_CRSEL            = 0xF7,  // CrSel key
00150                 KEY_EXSEL            = 0xF8,  // ExSel key
00151                 KEY_EREOF            = 0xF9,  // Erase EOF key
00152                 KEY_PLAY             = 0xFA,  // Play key
KEY_ZOOM             = 0xFB,  // Zoom key
KEY_PA1              = 0xFD,  // PA1 key
KEY_OEM_CLEAR        = 0xFE,   // Clear key

KEY_KEY_CODES_COUNT  = 0xFF // this is not a key, but the amount of keycodes there are.
};
*/

#endif
