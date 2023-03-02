{
  "targets": [
    {
      "target_name": "keymapping",
      "sources": [
        "src/string_conversion.cc",
        "src/keymapping.cc"
      ],
      'msvs_configuration_attributes': {
        'SpectreMitigation': 'Spectre'
      },
      'msvs_settings': {
        'VCCLCompilerTool': {
          'AdditionalOptions': [
            '/guard:cf',
            '/we4244',
            '/we4267',
            '/ZH:SHA_256'
          ]
        },
        'VCLinkerTool': {
          'AdditionalOptions': [
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
