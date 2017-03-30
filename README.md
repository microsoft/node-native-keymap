# OS key mapping node module [![Build Status](https://travis-ci.org/Microsoft/node-native-keymap.svg?branch=master)](https://travis-ci.org/Microsoft/node-native-keymap)
Returns what characters are produced by pressing keys with different modifiers on the current system keyboard layout.

## Installing

* On Debian-based Linux: `sudo apt-get install libx11-dev libxkbfile-dev`
* On Red Hat-based Linux: `sudo yum install libx11-devel.x86_64 libxkbfile-devel.x86_64 # or .i686`
* On FreeBSD: `sudo pkg install libX11`

```sh
npm install native-keymap
```

## Using

```javascript
var keymap = require('native-keymap');
console.log(keymap.getKeyMap());
```

Example output when using standard US keyboard layout:
```
[
  ...
  { key_code: 'VKEY_OEM_2',
    value: '/',
    withShift: '?',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_3',
    value: '`',
    withShift: '~',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_4',
    value: '[',
    withShift: '{',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_5',
    value: '\\',
    withShift: '|',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_6',
    value: ']',
    withShift: '}',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_7',
    value: '\'',
    withShift: '"',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_102',
    value: '\\',
    withShift: '|',
    withAltGr: '',
    withShiftAltGr: '' } ]
```

Example output when using German (Swiss) keyboard layout:
```
[
  ...
  { key_code: 'VKEY_OEM_2',
    value: '§',
    withShift: '°',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_3',
    value: '',
    withShift: '¨!',
    withAltGr: ']',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_4',
    value: '\'',
    withShift: '?',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_5',
    value: '´ä',
    withShift: 'à',
    withAltGr: '{',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_6',
    value: '',
    withShift: '^`',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_7',
    value: '~ö',
    withShift: 'é',
    withAltGr: '',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_8',
    value: '$',
    withShift: '£',
    withAltGr: '}',
    withShiftAltGr: '' },
  { key_code: 'VKEY_OEM_102',
    value: '<',
    withShift: '>',
    withAltGr: '\\',
    withShiftAltGr: '' } ]
```

## Supported OSes
 * linux (X11)
 * windows
 * mac
 * freebsd

## Developing
 * only tested on `node v6.x`
 * `npm install -g node-gyp`
 * `node-gyp configure` (for debugging use `node-gyp configure -d`)
 * `node-gyp build`
 * `npm test` (for debugging change `index.js` to load the node module from the `Debug` folder and press `F5`)

## License
[MIT](https://github.com/Microsoft/node-native-keymap/blob/master/License.txt)
