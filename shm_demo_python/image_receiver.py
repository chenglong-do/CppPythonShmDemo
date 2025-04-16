import time

import mmap
import numpy as np
import posix_ipc

from c_structs import *


class ImageReceiver:

    def __init__(self, sm_name):
        memory_size = ctypes.sizeof(SharedNNImage)
        print(f"Memory size: {memory_size} bytes")
        self.memory = posix_ipc.SharedMemory(f"/{sm_name}", posix_ipc.O_CREAT, size=memory_size)
        self.memory_data = mmap.mmap(self.memory.fd, self.memory.size)
        self.memory.close_fd()

        # Initialize the semaphore in the new shared memory
        self.semaphore = posix_ipc.Semaphore(f"/{sm_name}Semaphore", posix_ipc.O_CREAT, initial_value=1)

        print("Shared memory and semaphore initialized...")

    def __del__(self):
        # Close the shared memory for image
        # if hasattr(self, 'memory_data') and self.memory_data:
        #     self.memory_data.close()
        pass
        # Remove the semaphore
        # self.semaphore.unlink()

    def receive_image(self):
        print("Receiving image...")
        # Lock the semaphore
        try:
            self.semaphore.acquire(timeout=1)  # Try to acquire the semaphore with a timeout
            print("Semaphore acquired...")

            # 将共享内存内容映射到结构体
            shared_data = SharedNNImage.from_buffer(self.memory_data)

            print(f"is_processed: {shared_data.is_processed}")
            print(f"scale: {shared_data.scale}")
            print(f"width: {shared_data.width}, height: {shared_data.height}")

            if shared_data.is_processed == 1:
                print("Image is already processed")
                self.semaphore.release()
                return None

            # Check the size of the shared memory again before reading the image data
            if shared_data.width <= 0 or shared_data.height <= 0:
                # Unlock the semaphore
                self.semaphore.release()
                print("Invalid image size")
                time.sleep(1)
                return None

            # Read the image data
            img_data = np.ctypeslib.as_array(shared_data.data).reshape((shared_data.height, shared_data.width, 3))
            print("Image data shape: ", img_data.shape)

            # Mark the image as processed
            shared_data.is_processed = 1
            # Unlock the semaphore
            self.semaphore.release()

            return img_data
        except Exception as e:
            self.semaphore.release()
            print("Failed to receive image: ", e)
            time.sleep(1)
            return None
