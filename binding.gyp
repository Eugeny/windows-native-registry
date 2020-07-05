{
  "targets": [
    {
      "target_name": "native",
      "msvs_settings": {
        "VCCLCompilerTool": { "ExceptionHandling": 1 },
      },
      "sources": [
        "src/addon.cc",
      ],
      "libraries": [
        "advapi32",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
      ]
    }
  ]
}
