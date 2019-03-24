# Graphics by Lanea Zimmerman
# https://opengameart.org/content/tiny-16-basic
# Licensed under CC-BY 3.0, https://creativecommons.org/licenses/by/3.0/

import upygame

# *** GRAPHICS DATA

# map, 16x16
mapPixels1 = b'\
\x88\x88\x88\x88\x88\x88\x88\x88\
\x88\x88\x88\x88\x88\x88\x88\x88\
\x44\xbb\xbb\xb8\x8b\xbb\x55\xbb\
\x44\x5b\xbb\xb8\x8b\xb5\x55\x5b\
\x44\x4b\xbb\xb8\x8b\xb5\x55\x5b\
\x44\x4b\xb4\xbb\x88\xbb\x55\xbb\
\x44\x4b\x44\x4b\xb8\x8b\xbb\xbb\
\x44\x44\x45\x44\xbb\x88\x88\x88\
\x44\x4b\x44\x4b\xbb\xbb\xbb\x4b\
\x44\x44\xb4\xbb\xbb\xbb\xb4\x54\
\x44\x44\xbb\xbb\xbb\x44\xbb\x4b\
\x44\x44\x4b\xbb\xb4\x44\x4b\xbb\
\x44\x54\x4b\xbb\xb4\x44\x4b\xbb\
\x44\x44\x44\xb5\xbb\xb4\xbb\xbb\
\x44\x44\x44\xbb\xbb\xbb\xbb\xbb\
\x44\x44\x44\x4b\xbb\xbb\xbb\xbb\
'

green16Pixels = b'\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
'
green16 = upygame.surface.Surface(16, 16, green16Pixels)
tree16Pixels = b'\
\xbb\xbb\xb4\x44\x44\x4b\xbb\xbb\
\xbb\xb4\x44\x4b\x4b\x44\x4b\xbb\
\xbb\x44\x4b\xbb\xbb\xb4\x44\xbb\
\xbb\x11\x44\xbb\xbb\x44\x41\xbb\
\xb1\x14\x4b\x4b\xb4\xb4\x44\x1b\
\xb1\x41\x44\x4b\x44\x44\x11\x1b\
\xb1\x14\x41\x44\x44\x14\x41\x1b\
\xb4\x11\x14\x41\x44\x41\x41\x4b\
\xb1\x44\x14\x11\x41\x11\x11\x1b\
\xbb\x11\x11\x41\x11\x41\x41\xbb\
\xbb\xb1\x11\x41\x11\x14\x1b\xbb\
\xbb\xb7\x77\x11\x51\x77\x77\xbb\
\xbb\x77\x77\x15\x51\x77\x77\x7b\
\xbb\x77\x71\x55\x51\x17\x77\x7b\
\xbb\xb7\x77\x51\x55\x77\x77\xbb\
\xbb\xbb\x77\x77\x77\x77\xbb\xbb\
'
tree16 = upygame.surface.Surface(16, 16, tree16Pixels)

grass16Pixels = b'\
\xbb\xb4\xbb\xb4\x4b\xbb\xbb\xbb\
\x4b\xb4\xbb\xbb\xbb\x4b\xbb\xbb\
\xb4\xb4\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\x4b\xbb\xb4\xb4\
\xbb\x4b\xbb\xbb\x4b\x4b\xb4\xbb\
\x4b\x4b\xb4\xbb\x44\x4b\xbb\xbb\
\xb4\x44\x4b\xbb\x44\xbb\xbb\xbb\
\xb4\x4b\x4b\xbb\xbb\xbb\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xb4\xbb\xbb\
\xbb\xbb\xbb\xbb\xbb\xb4\xb4\xbb\
\xbb\xb4\xbb\xbb\xb4\xb4\x4b\xbb\
\xbb\xb4\xb4\x4b\xbb\x44\x4b\xbb\
\xbb\xb4\x44\xbb\xbb\xbb\xbb\xbb\
\xbb\x4b\xb4\xbb\xb4\xbb\xbb\xbb\
\xbb\xbb\xbb\xb4\xb4\xb4\xbb\xbb\
'
grass16 = upygame.surface.Surface(16, 16, grass16Pixels)


water16Pixels = b'\
\x8c\x88\x88\x8c\x88\x8c\x88\x88\
\xff\xc8\x88\x88\x88\x88\xc8\x8c\
\x88\x88\xcc\x88\x88\x88\x8c\xc8\
\x88\x88\x8f\x88\x88\x88\x8c\x88\
\x88\x88\x8f\x88\x88\x88\xc8\x88\
\x88\x8c\xcc\xc8\x88\x8c\xf8\x88\
\x88\xf8\x88\x88\xcc\x88\xc8\x88\
\x8c\xc8\x88\x88\xc8\x88\x8c\x88\
\x88\x8c\x88\x88\x88\x88\x88\xcc\
\x88\x88\xc8\x8c\x88\x88\x88\xf8\
\x88\x88\xcc\xc8\x88\x88\x88\xc8\
\xc8\x88\x88\xc8\x88\x88\x88\x8c\
\x8c\xc8\x88\x88\xc8\x88\xc8\x88\
\x8c\x88\x88\x88\x8c\xff\x88\x88\
\x88\x88\x88\x88\x88\xc8\x88\x88\
\x8c\x88\x88\x88\xfc\xcc\x88\x88\
'
water16 = upygame.surface.Surface(16, 16, water16Pixels)

girl12x15Pixels = b'\
\x00\x01\x11\x11\x10\x00\
\x00\x15\x99\x99\x51\x00\
\x01\x59\x59\xe1\x55\x10\
\x01\xe5\xee\x11\x95\x10\
\x19\x59\x11\x19\x19\x51\
\x15\x51\x1e\xe1\x91\x51\
\x15\x1e\x1f\xf1\xe1\x51\
\x15\x15\xdd\xdd\x51\x51\
\x11\x18\x59\x95\x81\x11\
\x01\x82\xf8\x8f\x28\x10\
\x01\xd1\x2f\xf2\x1d\x10\
\x00\x12\x8f\xf8\x21\x00\
\x00\x18\x8c\xc8\x81\x00\
\x07\x11\xcc\xcc\x11\x70\
\x07\x77\x11\x11\x77\x70\
'
girl12x15 = upygame.surface.Surface(12, 15, girl12x15Pixels)


# *** AUDIO DATA

# You can just drag and drop an audio file here, e.g. a wav file.

# jump2.wav by LloydEvans09 at Freesound.org
# Licensed under CC-BY 3.0, https://creativecommons.org/licenses/by/3.0/
vup_sound = b'\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x66\x66\x66\x67\x77\x77\x77\x88\x88\x88\x88\x77\x77\x76\x66\
\x66\x66\x66\x67\x77\x77\x88\x88\x88\x88\x77\x77\x66\x66\x55\x56\x66\x77\x77\x88\
\x89\x98\x88\x77\x76\x66\x55\x55\x56\x67\x77\x88\x99\x99\x88\x77\x66\x65\x55\x55\
\x66\x77\x88\x99\x99\x98\x77\x66\x55\x44\x55\x67\x78\x89\x99\x98\x87\x66\x54\x44\
\x55\x67\x88\x9a\xa9\x88\x76\x54\x44\x45\x67\x88\x9a\xa9\x87\x66\x54\x34\x56\x78\
\x89\xaa\x98\x76\x54\x44\x45\x67\x89\xaa\x98\x76\x54\x44\x56\x78\x9a\xa9\x86\x54\
\x44\x56\x78\x9a\xa9\x86\x54\x44\x56\x78\x9a\x98\x76\x54\x45\x67\x89\xa9\x87\x54\
\x44\x57\x89\xaa\x87\x64\x44\x57\x89\xa9\x87\x54\x45\x67\x9a\xa8\x76\x44\x46\x79\
\xaa\x87\x54\x45\x68\x9a\x98\x65\x45\x67\x89\x88\x76\x56\x67\x88\x87\x76\x66\x77\
\x88\x77\x76\x67\x77\x77\x77\x77\x77\x77\x77\x77\x77\
'

#  Boss Fight.wav by Deathbygeko at Freesound.org
# Licensed under CC-BY 3.0, https://creativecommons.org/licenses/by/3.0/
sound2 = b'\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x7b\xee\xee\
\xed\x86\x76\x76\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x68\xde\xee\xde\xb6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x67\x67\x67\x67\x66\xbe\xde\xde\xd8\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x76\x8d\
\xed\xdd\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x6b\xed\xdd\xed\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\x68\xde\xdd\xde\xa6\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\xbe\xdd\xdd\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x8d\xed\xdd\xea\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6b\xed\xdd\xdd\x85\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x68\xde\xdd\xde\xa6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xdd\xdd\xd8\x56\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x8d\xed\xdd\
\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x6b\xed\xdd\xdd\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x68\xde\xdd\xde\xa6\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\
\xdd\xdd\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6b\xed\xea\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x69\xdd\xec\x76\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\xce\xdd\x85\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xde\xa6\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x9e\xde\xc7\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x7c\xed\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6b\
\xed\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x69\xed\xec\x76\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\xce\xdd\x85\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xde\xa6\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x9e\xde\xc7\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x7d\xed\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x6b\xed\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x69\
\xed\xec\x76\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\xde\xdd\x85\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xde\xa6\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x9e\xde\xc7\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x7d\xed\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x6b\xed\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x69\xed\xec\x76\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\
\xde\xdd\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xde\xa6\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x9e\xde\xc7\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x7d\xed\xd8\x56\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x8d\xed\xe9\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x6b\xed\xeb\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x69\xed\xdd\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x67\xde\xde\x96\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\xbe\xde\xb6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x9e\xdd\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x7d\xed\
\xe9\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6b\xed\xeb\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x69\xed\xdd\x85\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\xde\xde\x96\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xde\xb6\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x9e\xdd\xd8\x56\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x7d\xed\xe9\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x6b\xed\xeb\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x69\xed\xdd\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x67\xde\xde\xa6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\xbe\xde\xb6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x9e\xdd\
\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x7d\xed\xea\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6b\xed\xeb\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x69\xed\xdd\x85\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\xde\xde\xa6\x66\x66\x69\xdd\xdd\x95\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\xbe\xde\xc7\x56\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x9d\xdd\xea\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6b\xed\xec\x75\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x68\xde\xde\xa6\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xdd\xd8\x56\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x8d\xed\xeb\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6a\xed\xdd\x85\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\xce\xde\xb6\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x9e\xdd\xd9\x56\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x7c\xed\xec\x76\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x69\xdd\xde\xa6\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xde\xc7\x56\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x8d\xed\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x6b\xed\xed\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x68\xde\xde\xb6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\xae\xdd\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x7c\xed\xeb\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x6a\xed\xdd\x95\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x67\xce\xde\xc7\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x9e\xdd\xe9\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x68\xde\xde\xc7\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x8d\xed\xec\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x69\xed\xde\xb6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\xae\xdd\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x6b\xed\xdd\x95\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x67\xce\xdd\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x7d\xed\xec\x75\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x68\xde\xde\xc7\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x9d\xdd\xeb\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x6a\xed\xde\xa6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\xbe\xdd\xd9\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x7c\xed\xdd\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x67\xce\xde\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x8d\xed\xec\x76\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x69\xde\xde\xb6\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\xae\xdd\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x6a\xed\xde\x96\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xdd\xd9\x56\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x7c\xed\xed\x85\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x68\xde\xde\xc7\x56\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x8d\xed\xeb\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x69\xed\xde\xb6\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xae\xdd\xea\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6b\xed\xdd\x95\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\xce\xdd\xd8\x56\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x7d\xed\xec\x75\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x68\xde\xde\xc7\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x9e\xdd\xeb\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6a\xed\xde\xa6\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xdd\xd9\x56\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x7c\xed\xdd\x85\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\xce\xde\xd7\x56\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x8d\xed\xec\x76\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x69\xdd\xde\xb6\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xae\xdd\xea\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6b\xed\xde\x95\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xbe\xdd\xd9\
\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x7c\xed\xed\
\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x68\xde\xde\
\xc7\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x9d\xed\
\xeb\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x69\xed\
\xde\xb6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\xae\
\xdd\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x6b\
\xed\xdd\x95\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x67\
\xce\xde\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x8d\xed\xec\x75\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x68\xde\xde\xc6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x9e\xdd\xeb\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x6a\xed\xde\xa6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\xbe\xdd\xd9\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x7c\xed\xdd\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x67\xce\xde\xc7\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x8d\xed\xec\x76\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x69\xdd\xde\xb6\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\xae\xdd\xea\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x6b\xed\xdd\x95\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x67\xbe\xdd\xd8\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x7c\xed\xed\x85\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x68\xde\xde\xc7\x56\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x9d\xed\xeb\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\x66\
\x66\x66\x66\x66\x67\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\x77\
\
'
