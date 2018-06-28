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

    return mean([int(r) for r in results])

with open("results.txt", "w+") as results_file:
    results_file.write("Num of threads,Elapsed time\n")
    for t in range(1, 17):
        results_file.write(str(t) + "," + str(calculate_average(t)) + "\n")
