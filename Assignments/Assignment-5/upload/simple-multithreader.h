#include <iostream>
#include <list>
#include <functional>
#include <chrono>
#include <pthread.h>
#include <vector>

int user_main(int argc, char **argv);

double totalExecutionTime = 0.0; // global variable to store the total execution time of parallel_for calls
std::vector<double> executionTimes; // vector to store individual execution times

void demonstration(std::function<void()> &&lambda) {
  lambda();
}

// args for thread_func
struct thread_args {
  enum Type { // 2 types of thread_args
    TYPE_1,
    TYPE_2
  };

  int low; // TYPE_1
  int high; // TYPE_1
  int low1; // TYPE_2
  int high1; // TYPE_2
  int low2; // TYPE_2
  int high2; // TYPE_2

  Type type;
  std::function<void(int)> myLambda1; // TYPE_1
  std::function<void(int, int)> myLambda2; // TYPE_2
};

// thread function
void* thread_func(void* ptr) {
  thread_args* t = (thread_args*)ptr;

  switch (t->type) {
    case thread_args::TYPE_1:
      // for TYPE_1, low and high are used
      for (int i = t->low; i < t->high; i++) {
        t->myLambda1(i);
      }

      break;

    case thread_args::TYPE_2:
      // for TYPE_2, low1, high1, low2, and high2 are used
      for (int i = t->low1; i < t->high1; i++) {
        for (int j = t->low2; j < t->high2; j++) {
          t->myLambda2(i, j);
        }
      }

      break;

    default:
      std::cerr << "Unknown type" << std::endl;
      break;
  }

  return NULL;
}

void parallel_for(int low, int high, std::function<void(int)> &&lambda, int numThreads) {
  auto start_time = std::chrono::high_resolution_clock::now(); // start time

  pthread_t tid[numThreads]; // array of thread ids
  thread_args args[numThreads]; // array of type thread_args

  int size = high - low;
  int chunk = size / numThreads;
  int remainder = size % numThreads; // in case size is not divisible by numThreads

  for (int i = 0; i < numThreads; i++) {
    args[i].low = low + i * chunk;
    args[i].high = low + (i + 1) * chunk + (i == numThreads - 1 ? remainder : 0); // last thread takes the remainder
    args[i].type = thread_args::TYPE_1;
    args[i].myLambda1 = lambda;

    if (pthread_create(&tid[i], NULL, thread_func, (void*)&args[i]) != 0) {
      std::cerr << "Error creating thread" << std::endl;
    }
  }

  for (int i = 0; i < numThreads; i++) {
    if (pthread_join(tid[i], NULL) != 0) {
      std::cerr << "Error joining thread" << std::endl;
    }
  }

  auto end_time = std::chrono::high_resolution_clock::now(); // end time
  std::chrono::duration<double> elapsed_time = end_time - start_time;
  executionTimes.push_back(elapsed_time.count());
  totalExecutionTime += elapsed_time.count();
}

void parallel_for(int low1, int high1, int low2, int high2, std::function<void(int, int)> &&lambda, int numThreads) {
  auto start_time = std::chrono::high_resolution_clock::now();

  pthread_t tid[numThreads]; // array of thread ids
  thread_args args[numThreads]; // array of type thread_args

  int size = high1 - low1;
  int chunk = size / numThreads;
  int remainder = size % numThreads; // in case size is not divisible by numThreads

  for (int i = 0; i < numThreads; i++) {
    args[i].low1 = low1 + i * chunk;
    args[i].high1 = low1 + (i + 1) * chunk + (i == numThreads - 1 ? remainder : 0); // last thread takes the remainder
    args[i].low2 = low2;
    args[i].high2 = high2;
    args[i].type = thread_args::TYPE_2;
    args[i].myLambda2 = lambda;

    if (pthread_create(&tid[i], NULL, thread_func, (void*)&args[i]) != 0) {
      std::cerr << "Error creating thread" << std::endl;
    }
  }

  for (int i = 0; i < numThreads; i++) {
    if (pthread_join(tid[i], NULL) != 0) {
      std::cerr << "Error joining thread" << std::endl;
    }
  }

  auto end_time = std::chrono::high_resolution_clock::now(); // end time
  std::chrono::duration<double> elapsed_time = end_time - start_time;
  executionTimes.push_back(elapsed_time.count());
  totalExecutionTime += elapsed_time.count();
}

int main(int argc, char **argv) {
  int x = 5, y = 1;

  auto lambda1 = [x, &y](void) {
    y = 5;
    std::cout << "====== Welcome to Assignment-" << y << " of the CSE231(A) ======\n";
  };

  demonstration(lambda1);

  int rc = user_main(argc, argv);

  // Display individual execution times
  for (int i = 0; i < executionTimes.size(); i++) {
    std::cout << "Execution time for parallel_for call " << i + 1 << ": " << executionTimes[i] << " seconds\n";
  }

  // Display the total execution time
  std::cout << "Total execution time for parallel_for calls: " << totalExecutionTime << " seconds\n";

  auto lambda2 = []() {
    std::cout << "====== Hope you enjoyed CSE231(A) ======\n";
  };

  demonstration(lambda2);

  return rc;
}

#define main user_main