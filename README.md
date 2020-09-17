# OS key mapping node module [![Build Status](https://dev.azure.com/ms/node-native-keymap/_apis/build/status/microsoft.node-native-keymap?branchName=master)](https://dev.azure.com/ms/node-native-keymap/_build/latest?definitionId=138&branchName=master)
Returns what characters are produced by pressing keys with different modifiers on the current system keyboard layout.

## Installing

* On Debian-based Linux: `sudo apt-get install libx11-dev libxkbfile-dev`
* On Red Hat-based Linux: `sudo yum install libx11-devel.x86_64 libxkbfile-devel.x86_64 # or .i686`
* On SUSE-based Linux: `sudo zypper install libX11-devel libxkbfile-devel`
* On FreeBSD: `sudo pkg install libX11`

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
 * linux (X11)
 * windows
 * mac
 * freebsd

## Known issues
 * only tested from the Electron Main process

## Developing
 * `npm install -g node-gyp`
 * `node-gyp configure` (for debugging use `node-gyp configure -d`)
 * `node-gyp build`
 * `npm test` (for debugging change `index.js` to load the node module from the `Debug` folder and press `F5`)

## License
[MIT](https://github.com/Microsoft/node-native-keymap/blob/master/License.txt)
