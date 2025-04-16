#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>
#include <stdexcept>

template <typename T>
class SharedMemoryTool {
public:
  explicit SharedMemoryTool(const std::string& sharedMemoryName) : sharedMemoryName(sharedMemoryName) {
    shm_unlink(sharedMemoryName.c_str());
    sem_unlink((sharedMemoryName + "Semaphore").c_str());

    std::cout << "Initializing shared memory: " << sharedMemoryName << " with size: " << sizeof(T) << std::endl;

    static_assert(alignof(T) <= 16, "Structure alignment requirement too large");
    std::cout << "Structure alignment: " << alignof(T) << ", size: " << sizeof(T) << std::endl;

    // Create shared memory
    memory_fd = shm_open(sharedMemoryName.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (memory_fd == -1) {
      throw std::runtime_error("Failed to open shared memory.");
    }

    // Set the size of the shared memory
    if (ftruncate(memory_fd, sizeof(T)) == -1) {
      throw std::runtime_error("Failed to set size of shared memory.");
    }

    // Map the shared memory
    sharedData = static_cast<T*>(mmap(nullptr, sizeof(T), PROT_READ | PROT_WRITE, MAP_SHARED, memory_fd, 0));
    if (sharedData == MAP_FAILED) {
      throw std::runtime_error("Failed to map shared memory.");
    }

    // Add memory accessibility test
    try {
      memset(sharedData, 0, sizeof(T));
    } catch (...) {
      munmap(sharedData, sizeof(T));
      throw std::runtime_error("Failed to access shared memory");
    }

    semaphore = sem_open((sharedMemoryName + "Semaphore").c_str(), O_CREAT | O_RDWR, 0666, 1);
  }

  ~SharedMemoryTool() {
    // Unmap the shared memory
    munmap(sharedData, sizeof(T));
    sem_close(semaphore);

    // Close the shared memory
    close(memory_fd);
  }

  void writeData(const T* dataPtr) const {
    sem_wait(semaphore);
    try {
      memcpy(sharedData, dataPtr, sizeof(T));
    } catch (...) {
      sem_post(semaphore);
      throw;
    }
    sem_post(semaphore);
  }

  std::unique_ptr<T> readData() const {
    if (sharedData == nullptr || sharedData == MAP_FAILED) {
      throw std::runtime_error("Invalid shared memory mapping");
    }
    sem_wait(semaphore);
    auto dataPtr = std::make_unique<T>(); // 动态分配在堆上
    try {
      memcpy(dataPtr.get(), sharedData, sizeof(T));
    } catch (...) {
      sem_post(semaphore);
      throw std::runtime_error("Failed to copy data from shared memory");
    }
    sem_post(semaphore);
    return dataPtr;
  }

private:
  int memory_fd;
  T* sharedData;
  sem_t* semaphore;
  std::string sharedMemoryName;
};