python tools/mpy-tool.py -mlongint-impl=none -f -q  genhdr/qstrdefs.preprocessed.h mpy\data.mpy mpy\gameobject.mpy mpy\globals.mpy mpy\main.mpy mpy\rooms.mpy mpy\sprite.mpy > frozen_mpy.c