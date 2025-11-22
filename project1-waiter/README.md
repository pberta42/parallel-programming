# 🍽️ Parallel Restaurant Simulation — Assignment Description

This project simulates a restaurant workflow using concurrency.  
The assignment must be implemented **in both Java and C**, using **only mutexes and condition variables** for synchronization.

---

## 📘 Task Description

Simulate the following scenario:

Five waiters serve customers in a restaurant. Four cooks prepare meals in the kitchen.

- A waiter needs some time to bring an order from a table to the kitchen (**1s in simulation**).  
- Then the waiter waits for a cook, who takes some time to prepare the meal (**2s in simulation**).  
- After the meal is ready, the waiter delivers it back to the customer (**1s in simulation**).  
- The simulation should run for a fixed duration (**30s**).

### 1️⃣ Requirements
1. Add a counter for the **total number of meals cooked**.  
   Each waiter should also track **how many orders they handled**.  
   At the end of the simulation, print these counters.

2. When a waiter completes **2 orders**, they wait for two more waiters to also complete 2 orders, and then they **take a break (2s in simulation)**.

3. Ensure the program **finishes cleanly** right after the allotted simulation time ends  
   (no new actions should begin once the time expires).

---

## 📝 Notes
- Use **only mutexes and condition variables** for synchronization.  
- Do **not rely on the specific times** given (1s, 2s, 1s).  
  The simulation should work correctly even if the timing changes.  
- Ensure proper thread termination and prevent new work from starting after time runs out.

---

## 🛠️ Implementation Requirements

You must implement this assignment **in both Java and C**.

### ✔️ Java Version
Use:
- `Thread` or `ExecutorService`
- `ReentrantLock`
- `Condition`
- Shared state protected by locks
- Time tracking with `System.currentTimeMillis()`


### ✔️ C Version
Use:
- `pthread_t`
- `pthread_mutex_t`
- `pthread_cond_t`
- Shared counters stored in a synchronized structure
- Time tracking with `clock_gettime()` or similar
