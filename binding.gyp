{
  "targets": [
    {
      "target_name": "keymapping",
      "sources": [
        "src/string_conversion.cc",
        "src/keymapping.cc"
      ],
      'cflags': [
        '-O2', '-fstack-protector-strong'
      ],
      'msvs_configuration_attributes': {
        'SpectreMitigation': 'Spectre'
      },
      'msvs_settings': {
        'VCCLCompilerTool': {
          'AdditionalOptions': [
            '/guard:cf',
            '/sdl',
            '/W3',
            '/we4146',
            '/we4244',
            '/we4267',
            '/ZH:SHA_256'
          ]
        },
        'VCLinkerTool': {
          'AdditionalOptions': [
            '/DYNAMICBASE',
            '/guard:cf'
          ]
        }
      },
      "conditions": [
        ['OS=="linux"', {
          "sources": [
            "deps/chromium/x/keysym_to_unicode.cc",
            "src/keyboard_x.cc"
          ],
          "include_dirs": [
            "<!@(${PKG_CONFIG:-pkg-config} x11 xkbfile --cflags | sed s/-I//g)"
          ],
          "libraries": [
            "<!@(${PKG_CONFIG:-pkg-config} x11 xkbfile --libs)"
          ],
          "variables": {
            # Read the keymap from the Wayland compositor when one is running.
            # Built when libwayland-client and libxkbcommon are found; set
            # NATIVE_KEYMAP_WAYLAND=0 to build the X11 backend only, or
            # NATIVE_KEYMAP_WAYLAND=1 to fail when the libraries are missing.
            "native_keymap_wayland%": "<!(if [ \"${NATIVE_KEYMAP_WAYLAND:-auto}\" = 0 ]; then echo 0; elif ${PKG_CONFIG:-pkg-config} --exists wayland-client xkbcommon; then echo 1; elif [ \"${NATIVE_KEYMAP_WAYLAND:-auto}\" = 1 ]; then echo 'NATIVE_KEYMAP_WAYLAND=1 needs libwayland-dev and libxkbcommon-dev' >&2; exit 1; else echo 0; fi)"
          },
          "conditions": [
            ['native_keymap_wayland in (1, "1")', {
              "sources": [
                "src/keyboard_wayland.cc"
              ],
              "defines": [
                "NATIVE_KEYMAP_WAYLAND"
              ],
              "include_dirs": [
                "<!@(${PKG_CONFIG:-pkg-config} wayland-client xkbcommon --cflags | sed s/-I//g)"
              ],
              "libraries": [
                "<!@(${PKG_CONFIG:-pkg-config} wayland-client xkbcommon --libs)"
              ]
            }]
          ]
        }],
        ['OS=="freebsd"', {
          "sources": [
            "deps/chromium/x/keysym_to_unicode.cc",
            "src/keyboard_x.cc"
          ],
          "include_dirs": [
            "/usr/local/include"
          ],
          "link_settings": {
            "libraries": [
              "-lX11",
              "-lxkbfile",
              "-L/usr/local/lib"
            ]
          }
        }],
        ['OS=="win"', {
          "sources": [
            "src/keyboard_win.cc"
          ]
        }],
        ['OS=="mac"', {
          "sources": [
            "src/keyboard_mac.mm"
          ],
          'link_settings' : {
            'libraries' : [
              '-framework Cocoa'
            ]
          }
        }],
        ['OS=="aix"', {
          "sources": [
            "deps/chromium/x/keysym_to_unicode.cc",
            "src/keyboard_x.cc"
          ],
          "link_settings": {
            "libraries": [
              "-lX11",
              "-lxkbfile"
            ]
          }
        }]
      ]
    }
  ]
}
