---
layout: page
title: Lab 2
parent: DSA Labs
grand_parent: Domain Specific Accelerators
nav_exclude: true
---

# Lab 2: Matrix-Multiply on the ARM

In this lab, you will implement a matrix-multiply application on the ARM CPU of the Zynq Soc. This lesson will transition you from theoretical linear algebra to practical high-performance computing (HPC). It emphasizes that data movement, not just operations count, dictates performance. By focusing on CPU hierarchy *(L1/L2/L3 caches)* interacts with memory layouts (row-major vs column-major) and blocking techniques, you will learn how to optimize data access patterns and write "cache-aware" code using techniques like loop reordering and tiling to minimize expensive trips to Main Memory (DRAM).

{: .info-title}
> **Why Matrix Multiplication?**
>
> Matrix multiplication is a fundamental operation in many scientific and engineering applications. In machine and deep learning applications for example, it is often the most computationally intensive part; the dense layer. Optimizing matrix multiplication can lead to significant performance improvements.

## Learning Objectives

- Understand the impact of memory hierarchy on performance.
- Implement matrix multiplication on the ARM CPU.
- Optimize data access patterns for better cache utilization.

## The Way of the Novice

{: .hint-title}
> (1) Memory Access Patterns:
> - **Row-major**: Elements in a row are stored next to each other in memory.
> - **Column-major**: Elements in a column are stored next to each other in memory.
> - **Strided Access**: Accessing elements with a fixed step size, which can lead to inefficient cache usage.

The most straightforward way to implement matrix multiplication is using three nested loops, `(i, j, k)`. This method is easy to understand and works correctly, but it often performs poorly due to inefficient use of the CPU cache.

```c
#define IDX(i, j, N) ((i) * (N) + (j))

void naive_matrix_multiply(int N, const float* A, const float* B, float* C) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            C[IDX(i, j, N)] = 0;
            for (int k = 0; k < N; k++)
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
        }
}
```

Observe, the `i, j, k` loop iterates through the rows of `A`, and columns of `B`.

order leads to poor spatial locality when accessing the elements of matrix `B`, which is stored in row-major order. This results in frequent cache misses and increased memory latency.

## Befriending the Cache 

{: .hint-title}
> (2) Cache Locality:
> - **Cache**: A small, fast memory located close to the CPU that stores frequently accessed data to reduce latency.
> - **Cache Miss**: When the CPU tries to access data that is not in the cache, resulting in a slower access from main memory.
> - **Spatial Locality**: Accessing data elements that are close to each other in memory.
> - **Temporal Locality**: Accessing the same data elements multiple times within a short period.

Modern CPUs don't pull single elements from memory; they pull entire cache lines (typically 64 bytes) that contain multiple elements. In a naive matrix multiplication, while you iterate through a row of **A** *(good spatial locality)*, you simultaneously jump down columns of **B** *(poor spatial locality)*. This causes a "Cache Miss" on almost every access to **B**. In general, if our access pattern doesn't align well with how data is stored in memory, we end up with many cache misses, which can significantly degrade performance. 

```c
void cache_aware_matrix_multiply(int N, const float* A, const float* B, float* C) {
    for (int i = 0; i < N; ++i)
        for (int k = 0; k < N; ++k) {
            float r = A[IDX(i, k, N)];
            for (int j = 0; j < N; ++j)
                C[IDX(i, j, N)] += r * B[IDX(k, j, N)];
        }
}
```

One of simplest optimizations is changing the loop order to `i, k, j`. By reordering the loops from `(i, j, k)` to `(i, k, j)`, we can improve spatial locality when accessing **B** and **C**, as we will be accessing elements in the same row consecutively.

- **Mechanism:** By making `j` the innermost loop, we access `B[k][j]` and `C[i][j]` in a way that is more cache-friendly, as we will be accessing elements in the same row consecutively.
- **Impact:** Once the CPU loads a cache line containing `B[k][j]`, the next several iterations of `j` will likely find their data already in the cache, reducing the number of cache misses and improving performance. **Spatial Locality** is improved by simply changing the memory access pattern. 

## The Beginning of the Mastery

{: .hint-title}
> (3) Tiling:
> - **Tiling**: A technique that breaks down a large matrix operation into smaller sub-matrices (tiles).
> - **Working Set**: The portion of data that is actively being processed at any given time.

Even with the loop reordering, for very large matrices, the data eventually exceeds the cache size, leading to cache evictions and subsequent cache misses. This is because the working set of data may not fit entirely in the cache. Tiling or blocking allows us to break down the matrix multiplication into smaller sub-matrices (tiles) that fit into the cache, enabling better reuse of data and reducing cache misses.

```c
#define TILE_SIZE 16
#define IDX(i, j, N) ((i) * (N) + (j))

void tiled_matrix_multiply(int N, const float* A, const float* B, float* C) {
    for (int i = 0; i < N; i += TILE_SIZE)
        for (int j = 0; j < N; j += TILE_SIZE)
            for (int k = 0; k < N; k += TILE_SIZE)
                // Compute the tile C[i:i+TILE_SIZE][j:j+TILE_SIZE]
                for (int ii = i; ii < i + TILE_SIZE && ii < N; ++ii)
                    for (int kk = k; kk < k + TILE_SIZE && kk < N; ++kk) {
                        float r = A[IDX(ii, kk, N)];
                        for (int jj = j; jj < j + TILE_SIZE && jj < N; ++jj)
                            C[IDX(ii, jj, N)] += r * B[IDX(kk, jj, N)];
                    }
}
```

- **Strategy:** Instead of processing the full row or column at once, we can process smaller blocks (tiles) of the matrix that fit into the cache. This way, we can maximize the reuse of data in the cache before it gets evicted.
- **Impact:** By processing smaller tiles, we maximize **Temporal Locality** by reusing the same cache resident data multiple times before moving on the next tile.

Combined with loop reordering, tiling can significantly improve the performance of matrix multiplication by improving both spatial and temporal locality, leading to fewer cache misses and faster execution.

## Exercise

### 1. Implementation

```c
void naive_matrix_multiply(int N, const float* A, const float* B, float* C);

void cache_aware_matrix_multiply(int N, const float* A, const float* B, float* C);

void tiled_matrix_multiply(int N, const float* A, const float* B, float* C);

void fill_matrix(int N, float* buffer);

void init_random() {
    XTime t_now;
    XTime_GetTime(&t_now);
    srand((unsigned)t_now);
}
```

Implement the three versions of matrix multiplication: naive, cache-aware, and tiled. Use the provided function signatures and fill in the implementations based on the snippets above. You'll also need to implement the `fill_matrix` function to initialize the matrices with random values.


### 2. Benchmarking

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xiltimer.h>

int main() {
    int N = 32; // Matrix size

    size_t matrix_size = (size_t)N * (size_t)N;
    float* A = (float*)malloc(matrix_size * sizeof(float));
    float* B = (float*)malloc(matrix_size * sizeof(float));
    float* C = (float*)malloc(matrix_size * sizeof(float));
    if (A == NULL || B == NULL || C == NULL) {
        printf("Failed to allocate matrices for N = %d\n", N);
        free(A);
        free(B);
        free(C);
        return 1;
    }

    init_random();
    fill_matrix(N, A);
    fill_matrix(N, B);
    memset(C, 0, matrix_size * sizeof(float)); // Initialize C to zero

    // Benchmark the three implementations here:
    XTime start, end;
    XTime_GetTime(&start);

    // Call the desired matrix multiplication function here, e.g.:
    // naive_matrix_multiply(N, A, B, C);

    XTime_GetTime(&end);
    double elapsed_time = (double)(end - start) / (COUNTS_PER_SECOND);
    printf("Elapsed time: %f seconds\n", elapsed_time);

    free(A);
    free(B);
    free(C);
    return 0;
}
```

Benchmark and record execution times of the three implementations for different values of `N` (e.g., 128, 256, 512) to observe how performance scales with matrix size. You can do this by measuring the execution time of each function. Use the `XTime` functions to get the start and end times, and calculate the elapsed time in seconds. Compare the results to see the performance differences between the naive, cache-aware, and tiled implementations.

{: .warning-title}
> Note: 
>
> Make sure to reset the output matrix **C** to zero before each multiplication to ensure accurate benchmarking.

**Expected Outcome:** You should see a significant decrease in execution time as you move from the naive implementation to the cache-aware and tiled implementations, especially as the matrix size increases. The performance improvement will be more pronounced for larger matrices, where the benefits of better cache utilization become more evident.

### 3. Analysis

1. Start by summarizing the execution times for each implementation and matrix size in a table or graph for better visualization.
2. Calculate GFLOPS (Giga Floating Point Operations Per Second) for each implementation to quantify the performance improvements.

    $$
    GFLOPS = \frac{2N^3}{\text{Execution Time} \times 10^9}
    $$

3. Find the "Cliff" - At what value of **N** do you start to see significant performance differences between the implementations?
    1. Why do you think this is the case?
    2. If you calculate the size of the working set for each implementation, how does it relate to the cache sizes of the ARM CPU?
4. Analyze the results of your benchmarks. Discuss how the performance changes with different matrix sizes and how the optimizations (loop reordering and tiling) impact the execution time.
5. Consider the following questions:
    1. How does the choice of data type (e.g., `float` vs `double`) affect the performance of matrix multiplication? Consider the impact on cache usage and memory bandwidth. Can we do better than `float`?
    2. What is OpenMP, and how can it be used to further optimize matrix multiplication on multicore CPUs? Discuss the potential benefits and challenges of parallelizing the matrix multiplication using OpenMP.

## Next Steps

In the next lab, we will explore how to implement matrix multiplication on the FPGA fabric of the Zynq SoC using High-Level Synthesis (HLS). This will allow us to leverage the parallelism and customizability of the FPGA to further optimize our matrix multiplication implementation. We will also compare the performance of our FPGA implementation with our CPU implementations to see the benefits of hardware acceleration for this computationally intensive task.

