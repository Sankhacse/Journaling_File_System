import matplotlib.pyplot as plt

labels = ['Normal Write', 'Journaled Write', 'Recovery Time']
times = [0.0012, 0.0025, 0.0003]
colors = ['blue', 'red', 'green']

plt.figure(figsize=(9, 6))
bars = plt.bar(labels, times, color=colors)

# Add the exact time values on top of each bar
for bar in bars:
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2, yval + 0.00005, 
             f'{yval}s', ha='center', va='bottom', fontweight='bold')

plt.ylabel('Time in seconds')
plt.title('File System Performance Analysis - Phase 6')
plt.grid(axis='y', linestyle='--', alpha=0.7)

# Ensure the y-axis has enough room for the labels
plt.ylim(0, max(times) * 1.2)

plt.savefig('performance_graph.png')
print("Graph generated as performance_graph.png with exact time labels.")
