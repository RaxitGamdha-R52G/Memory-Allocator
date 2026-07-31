# Memory Allocator

A custom memory allocator implemented entirely in **C**, built to explore how modern C runtime libraries manage dynamic memory internally.

The project recreates the core behavior of the standard C dynamic memory management APIs:

* `mem_alloc()` → equivalent to `malloc()`
* `mem_calloc()` → equivalent to `calloc()`
* `mem_realloc()` → equivalent to `realloc()`
* `mem_free()` → equivalent to `free()`

Rather than requesting memory from the operating system for every allocation, the allocator acquires larger regions of virtual memory and manages them internally. This approach significantly reduces the number of expensive system calls while providing efficient allocation, deallocation, and memory reuse.

---

# Design Philosophy

Dynamic memory allocation should minimize interactions with the operating system whenever possible.

Instead of mapping memory for every allocation request, the allocator requests large contiguous regions, called **Pages**, from the operating system using:

* `mmap()` for memory allocation
* `munmap()` for releasing unused pages

Once a page has been acquired, the allocator is responsible for managing every allocation within that page. Small allocation requests are therefore satisfied without invoking additional system calls, making allocation considerably more efficient.

---

# Memory Organization

The allocator divides memory into two primary components.

## Page

A **Page** represents a contiguous virtual memory region obtained from the operating system.

Each page contains:

* **Page Header**

  * Stores page metadata.
  * Maintains links to neighboring pages.
  * Stores a pointer to the first memory block.
* **Memory Region**

  * The remaining space available for allocation.

---

## Memory Block

Each allocation inside a page is represented by a **Memory Block**.

A block consists of:

* **Block Header**

  * Allocation size
  * Allocation status (free/in use)
  * Previous block pointer
  * Next block pointer
  * Pointer to the owning page
* **User Memory**

  * Memory returned to the application.

---

# Internal Data Structures

The allocator uses **Doubly Linked Lists** for both pages and memory blocks.

## Page List

All allocated pages are connected using a doubly linked list, allowing efficient insertion and removal when pages are created or released.

## Block List

Each page maintains its own doubly linked list of memory blocks.

This enables efficient:

* Block splitting
* Block coalescing
* Forward traversal
* Backward traversal
* Memory reuse

---

# Allocation Strategy

When a memory request arrives, the allocator follows these steps:

1. Validate the requested allocation size.
2. Normalize the requested size according to the allocator's alignment.
3. Search for a suitable free block using the **First-Fit** allocation strategy.
4. If a suitable block exists:

   * Split it if sufficient space remains.
   * Otherwise allocate the entire block.
5. If no suitable block exists:

   * Request a new page from the operating system.
   * Create an initial free block.
   * Satisfy the allocation request.

---

# Reallocation Strategy

When resizing an existing allocation, the allocator attempts to avoid unnecessary memory movement by following this order:

1. Return the same pointer if the requested size is unchanged.
2. Shrink the current block in place, splitting it when enough space remains for a new free block.
3. Expand the current block into an adjacent free block whenever possible.
4. If in-place expansion is not possible, allocate a new block, copy the existing data, free the old block, and return the new pointer.

---

# Deallocation Strategy

When memory is released:

1. Mark the block as free.
2. Merge adjacent free blocks to reduce fragmentation.
3. If the page becomes completely unused:

   * Release the page back to the operating system using `munmap()`.

---

# Memory Reuse

Instead of immediately requesting additional pages, the allocator attempts to reuse previously freed blocks.

This minimizes:

* Memory fragmentation
* Operating system calls
* Unnecessary virtual memory allocation

The allocator currently performs:

* First-Fit block search
* Block splitting
* Block coalescing
* Automatic page reclamation

---

# Current Features

* Custom `mem_alloc()`
* Custom `mem_calloc()`
* Custom `mem_realloc()`
* Custom `mem_free()`
* Page-based memory management
* First-Fit allocation strategy
* In-place block expansion
* In-place block shrinking
* Automatic block relocation when required
* Block splitting
* Adjacent block coalescing
* Automatic page release
* 8-byte memory alignment
* Allocation overflow protection
* Benchmark framework comparing against the standard library

---

# Future Work

The allocator is being developed incrementally.

Planned features include:

* Segregated free lists
* Explicit free lists
* Improved allocation strategies (Best-Fit, Next-Fit)
* Thread safety
* Fragmentation analysis
* Performance optimizations
* Allocation statistics and diagnostics
* Performance optimizations

---

This project is intended as an educational exploration of how modern memory allocators work internally, progressing from a simple page allocator toward a production-inspired allocator, emphasizing correctness, performance, and cleaner systems programming practice.
