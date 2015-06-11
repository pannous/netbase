#!/usr/bin/env PYTHONIOENCODING="utf-8" python
import codecs
import sys
file=sys.argv[1]
reload(sys)  # Reload does the trick!
sys.setdefaultencoding('UTF8')
# truncated \UXXXXXXXX escape?? sed \U(8byte) to \u(4byte) first!
# UnicodeEncodeError: 'ascii' codec can't encode character u'\xfc' in position? call with 
# PYTHONIOENCODING="utf-8" ./unescape-umlauts.py 

# for line in codecs.open(file, "r", encoding="unicode_escape"):
for line in open(file):
   # print line,
   print unicode(line, 'unicode_escape'),
