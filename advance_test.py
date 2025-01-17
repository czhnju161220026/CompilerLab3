import os
import hashlib

path = 'advance_log/'
files = os.listdir(path)
files.sort()
for f in files:
    if f.endswith('.log'):
        f_result = open('Test_advanced/%s' % f, 'rb')
        f_output = open('advance_log/%s' % f, 'rb')
        results = f_result.read()
        outputs = f_output.read()
        if hashlib.md5(results).hexdigest() == hashlib.md5(outputs).hexdigest():
            print("\033[32m %s passed \033[0m" % (f))
        else:
            print("\033[31m %s wrong \033[0m" % (f))
