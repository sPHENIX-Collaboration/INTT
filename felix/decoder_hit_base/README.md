# Hit Base Decoder for INTT evt files
We can use this decoder in commissioning. It's originally implemented by J. Bertaux.

## ver1
The original version.
It works well though some hits may be lost.

## ver2
It's based on the event-base decoder implemented by T. Hachiya (general_codes/hachiya/InttEventTree/).
Though the event-base decoder works perfectly, we still need a hit-base decoder to reuse old codes.
The hit-base decoder is also useful for calibration data.