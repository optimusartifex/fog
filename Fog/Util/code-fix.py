#!/usr/bin/env python

# This script will convert all TABs to SPACEs in the Fog root directory
# and all  sub-directories (recursive).
#
# Converted sequences:
#   - The \r\n sequence is converted to \n (To UNIX line-ending).
#   - The \t (TAB) sequence is converted to TAB_REPLACEMENT which should
#     be two spaces.
#
# Affected files:
#   - *.cmake
#   - *.cpp
#   - *.h

import os

TAB_REPLACEMENT = "  "

for root, dirs, files in os.walk("../Src"):
  for f in files:
    path = os.path.join(root, f)

    # Remove the stupid "._" files created by MAC.
    if (len(f) > 2 and f[0] == u'.' and f[1] == u'_') or file == u".DS_Store":
      print "Removing file: " + path
      os.remove(path)
      continue

    if f.lower().endswith(".cpp") or f.lower().endswith(".h") or f.lower().endswith(".cmake") or f.lower().endswith(".txt"):

      fh = open(path, "rb")
      data = fh.read()
      fh.close()

      fixed = False

      if " \n" in data:
        print "Fixing space before \\n in: " + path
        while True:
          oldl = len(data)
          data = data.replace(" \n", "\n")
          if oldl == len(data): break;

        fixed = True
      
      if "\r" in data:
        print "Fixing \\r\\n in: " + path
        data = data.replace("\r", "")
        fixed = True

      if "\t" in data:
        print "Fixing TABs in: " + path
        data = data.replace("\t", TAB_REPLACEMENT)
        fixed = True

      if fixed:
        fh = open(path, "wb")
        fh.truncate()
        fh.write(data)
        fh.close()
