{
  "targets": [
    {
      "target_name": "multithread",
      "sources": ["src/multithread.cc"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}

