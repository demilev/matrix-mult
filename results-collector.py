import subprocess
import re

# function for calculating mean of array
def mean(numbers):
    return float(sum(numbers)) / len(numbers)

# function for calculating the average time
# of execution with t threads
def calculate_average(t, number_of_executions=5):
    results = []

    for i in range(5):
        output = subprocess.check_output(["./matrix-mult.o -m 1024 -n 1024 -k 1024 -q -t " + str(t)], shell=True)
        m = re.search("\d+", output.split('\n')[1])
        results.append(m.group(0))
    
    return mean([int(r) for r in results])

# write results to txt file in csv format
with open("results.txt", "w+") as results_file:
    results_file.write("Num of threads,Elapsed time\n")
    for t in range(1, 17):
        results_file.write(str(t) + "," + str(calculate_average(t)) + "\n")
