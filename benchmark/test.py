from cStringIO import StringIO
from padnums import pprint_table

table = [["", "taste", "land speed", "life"],
         ["spam", 300101, 4, 1003],
         ["eggs", 105, 13, 42],
         ["lumberjacks", 13, 105, 10]]

out = StringIO()
pprint_table(out, table)
print out.getvalue()
