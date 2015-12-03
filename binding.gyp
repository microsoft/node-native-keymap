{
  "targets": [
    {
      "target_name": "keyboard",
      "sources": [
        "deps/chromium/x/keysym_to_unicode.cc",
        "src/keyboard.cc",
        "src/keyboard_x.cc"
      ],
      "link_settings": {
        "libraries": [
          "-lX11"
        ]
      }
    }
  ]
}