# CppPythonShmDemo

**A Cross-Language Shared Memory IPC Demo for C++ and Python** 

This project demonstrates efficient data communication between C++ and Python using shared memory, providing a lightweight and high-performance solution for real-time inter-process communication (IPC).

## Use Cases

1. Real-time sensor data streaming (e.g., cameras, LiDAR)
2. Integrating C++ performance-critical modules with Python ML/scripting layers.

## Quick Start

1. Clone the repository:
   
    ```shell
    git clone https://github.com/your_username/CppPythonShmDemo.git  
    ```
   
2. Build and run C++ component:
   
    ```shell
    cd shm_demo_cpp && mkdir build && cd build && cmake .. && make
    ./shm_demo_cpp
    ```

3. Install Python requirements and run:
   
    ```shell
    cd shm_demo_python
    pip install -r requirements.txt
    python main.py
    ```
    
