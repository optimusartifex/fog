Fog-Framework - SVN
===================

http://code.google.com/p/fog/

Introduction
============

Fog-Framework is a library designed for a cross-platform development in C++ language.
Main goal of the library is to provide very fast and accurate graphics output on all
supported operating systems and architectures. Currently Fog is not only graphics
library, it contains many other classes related to cross-platform development, but
many classes are related to 2d graphics.

Dependencies
============

Windows:
- Fog-Core
  - Advapi32       (direct   - linked)
  - Winmm          (direct   - linked)
- Fog-G2d
  - GdiPlus        (optional - runtime loading)
  - Gdi32          (direct   - linked)
- Fog-UI
  - Gdi32          (direct   - linked)
  - User32         (direct   - linked)
  - UxTheme        (optional - runtime loading)

Mac:
- Fog-Core
  - CoreFoundation (direct   - linked)
- Fog-G2d
  - CoreGraphics   (direct   - linked)
  - CoreText       (direct   - linked)
- Fog-UI
  - Cocoa/UIKit    (direct   - linked)

Linux/BSD:
- Fog-Core
  - pthreads       (direct   - linked)
  - libdl          (direct   - linked)
  - librt          (direct   - linked)
- Fog-G2d
  - libjpeg        (optional - runtime loading)
  - libpng         (optional - runtime loading)
  - Fontconfig     (optional - runtime loading)
  - Freetype2      (direct   - runtime loading)
- Fog-UI
  - X11            (optional - runtime loading)
