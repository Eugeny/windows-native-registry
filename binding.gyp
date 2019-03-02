{
  "targets": [
    {
      "target_name": "native",
      "sources": [
        "src/addon.cc",
      ],
      "libraries": [
        "advapi32",
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
