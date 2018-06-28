import subprocess
import re

execution_times = []

for i in range(1, 33):
    output = subprocess.check_output(["./matrix-mult.o -m 1024 -n 1024 -k 1024 -q -t " + str(i)], shell=True)
    m = re.search("\d+", output.split('\n')[1])
    execution_times.append(m.group(0))
    
print(execution_times)