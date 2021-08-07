project(
  'sndfilter',
  'c',
)

sndfilter_lib = library(
  'sndfilter',
  include_directories: include_directories('include'),
  sources: files(
    'src/biquad.c',
    'src/compressor.c',
    'src/main.c',
    'src/mem.c',
    'src/reverb.c',
    'src/snd.c',
    'src/wav.c',
  ),
  link_args: [
    '-lm',
    ]
)

sndfilter_dep = declare_dependency(
  link_with: sndfilter_lib,
  include_directories: include_directories('include'),
  link_args: [
    '-lm',
  ]
)
