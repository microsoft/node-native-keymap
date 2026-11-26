# OS key mapping node module [![Build Status](https://dev.azure.com/ms/node-native-keymap/_apis/build/status/microsoft.node-native-keymap?branchName=master)](https://dev.azure.com/ms/node-native-keymap/_build/latest?definitionId=138&branchName=master)
Returns what characters are produced by pressing keys with different modifiers on the current system keyboard layout.

## Installing

* On Debian-based Linux: `sudo apt-get install libx11-dev libxkbfile-dev libwayland-dev libxkbcommon-dev`
* On Red Hat-based Linux: `sudo yum install libx11-devel.x86_64 libxkbfile-devel.x86_64 # or .i686`
* On SUSE-based Linux: `sudo zypper install libX11-devel libxkbfile-devel`
* On FreeBSD: `sudo pkg install libX11 libxkbfile`

```sh
npm install native-keymap
```

## Using

```javascript
var keymap = require('native-keymap');
console.log(keymap.getKeyMap());
```

Example output when using standard US keyboard layout (on Windows):
```
[
  ...
  Space:        { vkey: 'VK_SPACE',      value: ' ',  withShift: ' ', withAltGr: '', withShiftAltGr: '' },
  Minus:        { vkey: 'VK_OEM_MINUS',  value: '-',  withShift: '_', withAltGr: '', withShiftAltGr: '' },
  Equal:        { vkey: 'VK_OEM_PLUS',   value: '=',  withShift: '+', withAltGr: '', withShiftAltGr: '' },
  BracketLeft:  { vkey: 'VK_OEM_4',      value: '[',  withShift: '{', withAltGr: '', withShiftAltGr: '' },
  BracketRight: { vkey: 'VK_OEM_6',      value: ']',  withShift: '}', withAltGr: '', withShiftAltGr: '' },
  Backslash:    { vkey: 'VK_OEM_5',      value: '\\', withShift: '|', withAltGr: '', withShiftAltGr: '' },
  Semicolon:    { vkey: 'VK_OEM_1',      value: ';',  withShift: ':', withAltGr: '', withShiftAltGr: '' },
  Quote:        { vkey: 'VK_OEM_7',      value: '\'', withShift: '"', withAltGr: '', withShiftAltGr: '' },
  Backquote:    { vkey: 'VK_OEM_3',      value: '`',  withShift: '~', withAltGr: '', withShiftAltGr: '' },
  Comma:        { vkey: 'VK_OEM_COMMA',  value: ',',  withShift: '<', withAltGr: '', withShiftAltGr: '' },
  Period:       { vkey: 'VK_OEM_PERIOD', value: '.',  withShift: '>', withAltGr: '', withShiftAltGr: '' },
  Slash:        { vkey: 'VK_OEM_2',      value: '/',  withShift: '?', withAltGr: '', withShiftAltGr: '' },
  ...
]
```

Example output when using German (Swiss) keyboard layout (on Windows):
```
[
  ...
  Space:        { vkey: 'VK_SPACE',      value: ' ',  withShift: ' ', withAltGr: '',  withShiftAltGr: '' },
  Minus:        { vkey: 'VK_OEM_4',      value: '\'', withShift: '?', withAltGr: '´', withShiftAltGr: '' },
  Equal:        { vkey: 'VK_OEM_6',      value: '^',  withShift: '`', withAltGr: '~', withShiftAltGr: '' },
  BracketLeft:  { vkey: 'VK_OEM_1',      value: 'ü',  withShift: 'è', withAltGr: '[', withShiftAltGr: '' },
  BracketRight: { vkey: 'VK_OEM_3',      value: '¨',  withShift: '!', withAltGr: ']', withShiftAltGr: '' },
  Backslash:    { vkey: 'VK_OEM_8',      value: '$',  withShift: '£', withAltGr: '}', withShiftAltGr: '' },
  Semicolon:    { vkey: 'VK_OEM_7',      value: 'ö',  withShift: 'é', withAltGr: '',  withShiftAltGr: '' },
  Quote:        { vkey: 'VK_OEM_5',      value: 'ä',  withShift: 'à', withAltGr: '{', withShiftAltGr: '' },
  Backquote:    { vkey: 'VK_OEM_2',      value: '§',  withShift: '°', withAltGr: '',  withShiftAltGr: '' },
  Comma:        { vkey: 'VK_OEM_COMMA',  value: ',',  withShift: ';', withAltGr: '',  withShiftAltGr: '' },
  Period:       { vkey: 'VK_OEM_PERIOD', value: '.',  withShift: ':', withAltGr: '',  withShiftAltGr: '' },
  Slash:        { vkey: 'VK_OEM_MINUS',  value: '-',  withShift: '_', withAltGr: '',  withShiftAltGr: '' },
  ...
]
```

## Supported OSes
 * linux (X11 and Wayland)
 * windows
 * mac
 * freebsd

## Linux: X11 and Wayland
On Linux the module reads the keyboard layout from the Wayland compositor when `WAYLAND_DISPLAY` is set and a compositor accepts the connection, and from the X server otherwise. The Wayland backend binds `wl_seat`, receives the XKB keymap the compositor sends with `wl_keyboard.keymap`, and resolves it with libxkbcommon, so it works on Wayland-only sessions and reports the layout the compositor actually uses, also when Electron runs as an XWayland client. A Wayland client without a focused surface doesn't learn which layout of the keymap is active, so a keymap with several layouts (e.g. `us,ru`) is read from the X server when one can be reached: Electron apps such as VS Code run under XWayland unless they are started with `--ozone-platform=wayland`, and XWayland tracks the active layout while one of their windows is focused. `onDidChangeKeyboardLayout` listens to both.

 * Set `NATIVE_KEYMAP_BACKEND=x11` to always use the X server, or `NATIVE_KEYMAP_BACKEND=wayland` to never fall back to it.
 * The Wayland backend is built when `pkg-config` finds `wayland-client` and `xkbcommon` (`sudo apt-get install libwayland-dev libxkbcommon-dev`); the module then needs `libwayland-client.so.0` and `libxkbcommon.so.0` at runtime. Build with `NATIVE_KEYMAP_WAYLAND=0` to leave it out, or with `NATIVE_KEYMAP_WAYLAND=1` to fail when the libraries are missing.
 * With `NATIVE_KEYMAP_BACKEND=wayland`, or without an X server, the active layout of a keymap with several layouts is unknown: `getCurrentKeyboardLayout().group` is always `0` and switching between its layouts fires no `onDidChangeKeyboardLayout` event. Changing the keymap itself does fire it.
 * When the connection to the compositor is lost, `onDidChangeKeyboardLayout` fires and the module reads the X server until it connects again; it tries every second and fires again once connected.
 * The layout names come from the keymap's `xkb_symbols` section; when the compositor doesn't name it, they are looked up by description in the XKB registry (`evdev.xml`). `model` and `options` come from `XKB_DEFAULT_MODEL` and `XKB_DEFAULT_OPTIONS`, as the keymap doesn't carry them.

## Known issues
 * only tested from the Electron Main process

## Developing
 * `npm install -g node-gyp`
 * `node-gyp configure` (for debugging use `node-gyp configure -d`)
 * `node-gyp build`
 * `npm test` (for debugging change `index.js` to load the node module from the `Debug` folder and press `F5`)

## License
[MIT](https://github.com/Microsoft/node-native-keymap/blob/master/License.txt)
