import numpy as np
import matplotlib.pyplot as plt
import csv

def read_csv(filename):
    data = []
    with open(filename, 'r') as file:
        csv_reader = csv.reader(file)
        for row in csv_reader:
            data.append(row)
    return data

def plot_line_graphs(data):
    priority_dict = {}
    for row in data[1:]:
        priority = int(row[2])
        if priority not in priority_dict:
            priority_dict[priority] = {'exec_time': [], 'wait_time': []}
        priority_dict[priority]['exec_time'].append(int(row[3]))
        priority_dict[priority]['wait_time'].append(int(row[4]))

    priorities = list(priority_dict.keys())
    exec_time_avgs = [np.mean(priority_dict[p]['exec_time']) for p in priorities]
    wait_time_avgs = [np.mean(priority_dict[p]['wait_time']) for p in priorities]

    plt.figure(figsize=(10, 5))
    plt.plot(priorities, exec_time_avgs, 'ro-', label='Execution Time')
    plt.plot(priorities, wait_time_avgs, 'bo-', label='Wait Time')

    plt.title('Effect of Job Priority on Job Scheduling')
    plt.xlabel('Priority')
    plt.ylabel('Average Time')
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    data = read_csv('terminated_processes.csv')
    plot_line_graphs(data)
