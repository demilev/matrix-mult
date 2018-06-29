import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# get the results from the txt file
results = pd.read_csv("results.txt")

# calculate speedup and efficency
results["Speedup"] = results["Elapsed time"][0] / results["Elapsed time"]
results["Efficiency"] = results["Speedup"] / results["Num of threads"]


# save results to excel sheet
writer = pd.ExcelWriter("results.xlsx")
results.to_excel(writer, "Sheet1")
writer.save()


# plot time execution
fig = plt.figure(figsize=(12,10))
fig.suptitle("Графика на времето за изпълнение(в милисекунди)", fontsize=20)
plt.xlabel("Брой нишки", fontsize=18)
plt.ylabel("Време за изпълнение", fontsize=18)
plt.yticks(np.arange(0, max(results["Elapsed time"]) + 1000, 1000))
plt.plot(results["Num of threads"], results["Elapsed time"], label="Време за изпълнение")
plt.legend(loc=8)
fig.savefig("times.png")


# plot speedup
fig = plt.figure(figsize=(12,10))
fig.suptitle("Графика на ускорението", fontsize=20)
plt.xlabel("Брой нишки", fontsize=18)
plt.ylabel("Ускорение", fontsize=18)
plt.plot(results["Num of threads"], results["Num of threads"], color="green", label="Идеално ускорение")
plt.plot(results["Num of threads"], results["Speedup"], color="red", label="Измерено ускорение")
plt.legend(loc=4)
fig.savefig("speedup.png")

# plot efficiency
fig = plt.figure(figsize=(12,10))
fig.suptitle("Графика на ефективността", fontsize=20)
axes = plt.gca()
axes.set_ylim([0.5,1.5])
plt.yticks(np.arange(0.5, 1.5, 0.1))
plt.xlabel("Брой нишки", fontsize=18)
plt.ylabel("Ефективност", fontsize=18)
plt.plot(results["Num of threads"], results["Efficiency"], label="Ефективност")
plt.legend(loc=8)
fig.savefig("efficiency.png")