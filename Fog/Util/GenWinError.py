#!/usr/bin/env python

src = open("WinError.h", "r")
buf = ""
line = src.readline()
last = 0

while line:
  if line.startswith("#define ERROR_"):
    parts = line.split()
    if len(parts) >= 3:
      code = parts[2]
      if code.endswith("L"):
        code = code[:-1]
      try:
        code = int(code)

        if code > last:
          while code > last + 1:
            last += 1
            buf += "/* %05u: %-61s */ _UNASSIGNED,\n" % (last, "")

          last += 1
          buf += "/* %05u: %-61s */ _UNASSIGNED,\n" % (last, parts[1])
      except:
        pass
  line = src.readline()

src.close()

dst = open("WinError_EXTRACTED.h", "w+")
dst.write(buf)
dst.close()
