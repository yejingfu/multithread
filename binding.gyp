{
  "targets": [
    {
      "target_name": "multithread",
      "sources": [
        "src/multithread.cc",
        "src/thread.cc",
        "src/util.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}

