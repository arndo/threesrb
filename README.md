threesrb
========

Threes ported for the free mp3-player replacement firmware Rockbox.

###Build

1. clone rockbox from github
2. copy the contents of theesrb over to ``./rockbox/apps/plugins`` except for ``./SUBDIRS`` and ``./bitmap/native/SOURCES``
3. merge ``./SUBDIRS`` and ``./bitmap/native/SOURCES`` with the existing ones
4. build rockbox as usual

###Supported players
+ Players with a color LCD supporting bitmaps and a resolution >= 128x128

### TODO
+ better random card drawing (allowing for a more balanced ratio of red and blue cards on the board)
+ Greyscale and B/W support
+ text-only mode for smaller displays and non bitmap-LCDs
+ menu with playback-controlls
+ Savestates
