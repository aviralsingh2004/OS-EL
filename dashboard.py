import os
import time
import psutil
import matplotlib.pyplot as plt
from collections import deque

DELAY_SECONDS = 2  # Increased delay time to 2 seconds

# Function to display memory usage for a process
def display_memory_usage(pid):
    try:
        process = psutil.Process(pid)
        memory_info = process.memory_info()
        print(f"Memory Usage for PID {pid}: {memory_info.rss / 1024} KB")
    except psutil.NoSuchProcess:
        print(f"Process with PID {pid} not found")

# Function to display CPU usage for a process
def display_cpu_usage(pid):
    try:
        process = psutil.Process(pid)
        cpu_percent = process.cpu_percent(interval=None)
        print(f"CPU Usage for PID {pid}: {cpu_percent:.2f}%")
    except psutil.NoSuchProcess:
        print(f"Process with PID {pid} not found")

# Function to display running processes and their resource usage
def display_running_processes():
    for proc in psutil.process_iter(['pid', 'name']):
        print(f"Process ID: {proc.info['pid']}\t Program Name: {proc.info['name']}")
        display_memory_usage(proc.info['pid'])
        display_cpu_usage(proc.info['pid'])

# Function to display memory information
def display_memory_info():
    mem = psutil.virtual_memory()
    print(f"Total RAM: {mem.total / (1024 * 1024)} MB")
    print(f"Free RAM: {mem.available / (1024 * 1024)} MB")
    print(f"Used RAM: {mem.used / (1024 * 1024)} MB")

# Function to display CPU information
def display_cpu_info():
    cpu_info = os.popen("cat /proc/cpuinfo | grep 'model name'").read().strip()
    print(f"CPU Model: {cpu_info.split(':')[1].strip()}")

# Function to display disk usage
def display_disk_usage():
    disk_usage = psutil.disk_usage('/')
    print(f"Total Disk Space: {disk_usage.total / (1024 * 1024 * 1024)} GB")
    print(f"Used Disk Space: {disk_usage.used / (1024 * 1024 * 1024)} GB")
    print(f"Free Disk Space: {disk_usage.free / (1024 * 1024 * 1024)} GB")

# Function to display network activity
def display_network_activity():
    net_io = psutil.net_io_counters(pernic=True)
    for interface, stats in net_io.items():
        print(f"Interface: {interface}\t RX bytes: {stats.bytes_recv}\t TX bytes: {stats.bytes_sent}")

# Main function for real-time monitoring and plotting
def main():
    cpu_data = deque([0] * 100, maxlen=100)  # Initialize deque for storing CPU usage history
    disk_data = deque([0] * 100, maxlen=100)  # Initialize deque for storing disk usage history
    network_data = deque([0] * 100, maxlen=100)  # Initialize deque for storing network activity history
    plt.ion()  # Turn on interactive mode
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1)
    fig.suptitle('System Resource Monitoring', fontsize=16)
    ax1.set_xlim(0, 100)
    ax1.set_ylim(0, 100)
    ax1.set_title('CPU Usage (%)')
    ax1.set_ylabel('CPU Usage (%)')
    line1, = ax1.plot([], [])
    ax2.set_xlim(0, 100)
    ax2.set_ylim(0, 100)
    ax2.set_title('Disk Usage (%)')
    ax2.set_ylabel('Disk Usage (%)')
    line2, = ax2.plot([], [])
    ax3.set_xlim(0, 100)
    ax3.set_ylim(0, 100)
    ax3.set_title('Network Activity (bytes)')
    ax3.set_ylabel('Bytes')
    line3, = ax3.plot([], [])
    
    while True:
        os.system('clear')
        print(" ____  _   _   _   ____    _    ____  _   _ ____   ___    _    ____  ____  ")
        print("/ ___|  _ \\| | | | |  _ \\  / \\  / ___|| 	| | | __ ) / _ \\  / \\  |  _ \\|  _ \\ ")
        print("| |   | |_) | | | | | | | |/ _ \\ \\___ \\| |_| |  _ \\| | | |/ _ \\ | |_) | | | |")
        print("| |___|  __/| |_| | | |_| / ___ \\ ___) |  _  | |_) | |_| / ___ \\|  _ <| |_| |")
        print(" \\____|_|    \\___/  |____/_/   \\_\\____/|_| |_|____/ \\___/_/   \\_\\_| \\_\\____/")
        
        print("\n==== Ongoing Processes ====")
        display_running_processes()

        print("\n==== Memory Dashboard ====")
        display_memory_info()

        print("\n==== Disk Usage ====")
        display_disk_usage()

        print("\n==== Network Activity ====")
        display_network_activity()

        cpu_usage = psutil.cpu_percent(interval=None)  # Get current CPU usage
        disk_usage = psutil.disk_usage('/').percent  # Get current disk usage
        network_bytes = psutil.net_io_counters().bytes_recv  # Get current network bytes received
        cpu_data.append(cpu_usage)  # Append CPU usage to CPU data deque
        disk_data.append(disk_usage)  # Append disk usage to disk data deque
        network_data.append(network_bytes)  # Append network bytes to network data deque
        line1.set_xdata(range(len(cpu_data)))
        line1.set_ydata(cpu_data)
        line2.set_xdata(range(len(disk_data)))
        line2.set_ydata(disk_data)
        line3.set_xdata(range(len(network_data)))
        line3.set_ydata(network_data)
        ax1.relim()
        ax1.autoscale_view()
        ax2.relim()
        ax2.autoscale_view()
        ax3.relim()
        ax3.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()
        time.sleep(DELAY_SECONDS)

# Run the main function
if __name__ == "__main__":
    main()

