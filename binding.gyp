{
  "targets": [
    {
      "target_name": "multithread",
      "source": ["src/multihread.cc"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}

