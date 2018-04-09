import numpy as np
import matplotlib.pyplot as plt

plot_file = open('plot_data.txt', 'r')

col_data = plot_file.readline()
rows, cols, ws_sz = col_data.split(" ")
rows = int(rows)
cols = int(cols)
ws_sz = int(ws_sz)
itr = 0
print("rows: ", rows, " cols: ", cols)
colors = np.random.randn(ws_sz)

col_data = plot_file.readline()

while col_data is not "":
	l_str = col_data.split(" ")
	l_str.pop()
	l_int = [int(x) for x in l_str]
	# print(itr, ": ", l_int)
	plt.scatter([itr for x in l_int], l_int, c=colors[0:len(l_int)], alpha = 0.5)
	col_data = plot_file.readline()
	itr += 1

plt.show()