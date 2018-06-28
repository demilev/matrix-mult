import subprocess
import re

execution_times = []

def mean(numbers):
    return float(sum(numbers)) / len(numbers)

def calculate_average(t, number_of_executions=5):
    results = []

    for i in range(5):
        output = subprocess.check_output(["./matrix-mult.o -m 512 -n 512 -k 512 -q -t " + str(t)], shell=True)
        m = re.search("\d+", output.split('\n')[1])
        results.append(m.group(0))
        print(output)

    return mean([int(r) for r in results])

for t in range(1, 3):
    execution_times.append(calculate_average(t))
    
print(execution_times)