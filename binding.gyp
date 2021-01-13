{
  "targets": [{
    "target_name": "vapash",
    "sources": [
      "./src/vapash.cc",
      "./src/vapash-src/src/libvapash/io.c",
      "./src/vapash-src/src/libvapash/internal.c",
      "./src/vapash-src/src/libvapash/sha3.c"
    ],
    "cflags_c": [
      "-std=gnu99",
      "-Wall",
      "-Wno-maybe-uninitialized",
      "-Wno-uninitialized",
      "-Wno-unused-function",
      "-Wextra"
    ],
    "cflags_cc+": [
      "-fexceptions",
      "-std=c++11"
    ],
    "cflags_cc!": [
      "-fno-exceptions"
    ],
    "include_dirs": [
      "./src/vapash-src/src",
      "<!(node -e \"require('nan')\")"
    ]
  }]
}
