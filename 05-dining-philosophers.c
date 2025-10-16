/*
 * Problem 5: Dining Philosophers (Advanced)
 *
 * Difficulty: Advanced
 * Topic: Deadlock Prevention
 *
 * PROBLEM DESCRIPTION:
 * ==================
 * Solve the famous Dining Philosophers problem. Five philosophers sit at a round table
 * with five chopsticks (one between each pair). A philosopher needs TWO chopsticks to eat.
 * Your task is to prevent deadlock while allowing philosophers to eat.
 *
 * REQUIREMENTS:
 * ============
 * 1. Five philosophers (threads)
 * 2. Five chopsticks (mutexes)
 * 3. Each philosopher tries to eat 3 times
 * 4. To eat, a philosopher needs BOTH left and right chopsticks
 * 5. After eating, philosopher puts down both chopsticks
 * 6. PREVENT DEADLOCK using asymmetric solution:
 *    - Philosophers 0-3: Pick left first, then right
 *    - Philosopher 4: Pick right first, then left
 *
 * DEADLOCK SCENARIO TO AVOID:
 * ===========================
 * If all philosophers pick up their left chopstick simultaneously,
 * they will wait forever for their right chopstick -> DEADLOCK!
 *
 * COMPILATION:
 * ===========
 * cl.exe 05-dining-philosophers.c
 */

#include <stdio.h>
#include <windows.h>

#define NUM_PHILOSOPHERS 5
#define NUM_MEALS 3

printf("Hello");

// TODO: Declare chopstick mutexes (HANDLE chopsticks[NUM_PHILOSOPHERS])
HANDLE chopsticksMutex[NUM_PHILOSOPHERS];

// Statistics
int mealsEaten[NUM_PHILOSOPHERS] = {0};
HANDLE statsMutex;

void PickupChopsticks(int philosopherId, int leftChopstick, int rightChopstick) {
    // TODO: Implement deadlock-free chopstick pickup using asymmetric solution
    // Hint: Last philosopher (philosopherId == NUM_PHILOSOPHERS - 1) should pick in reverse order
    if (philosopherId == NUM_PHILOSOPHERS - 1) {
        WaitForSingleObject(chopsticksMutex[rightChopstick], INFINITE);
        WaitForSingleObject(chopsticksMutex[leftChopstick], INFINITE);
    }
    else {
        WaitForSingleObject(chopsticksMutex[leftChopstick], INFINITE);
        WaitForSingleObject(chopsticksMutex[rightChopstick], INFINITE);
    }


    printf("[Philosopher %d] Picked both chopsticks\n", philosopherId);
}

void PutdownChopsticks(int philosopherId, int leftChopstick, int rightChopstick) {
    // TODO: Release both chopsticks
    ReleaseMutex(chopsticksMutex);

    printf("[Philosopher %d] Put down chopsticks %d and %d\n",
           philosopherId, leftChopstick, rightChopstick);
}

DWORD WINAPI Philosopher(LPVOID lpParam) {
    int id = (int)lpParam;
    int leftChopstick = id;
    int rightChopstick = (id + 1) % NUM_PHILOSOPHERS;

    for (int meal = 0; meal < NUM_MEALS; meal++) {
        // Think
        printf("[Philosopher %d] Thinking...\n", id);
        Sleep(rand() % 100);

        // Pick up chopsticks
        PickupChopsticks(id, leftChopstick, rightChopstick);

        // Eat
        printf("[Philosopher %d] Eating meal %d/%d\n", id, meal + 1, NUM_MEALS);
        Sleep(rand() % 100);

        WaitForSingleObject(statsMutex, INFINITE);
        mealsEaten[id]++;
        ReleaseMutex(statsMutex);

        // Put down chopsticks
        PutdownChopsticks(id, leftChopstick, rightChopstick);

        Sleep(rand() % 50);
    }

    printf("[Philosopher %d] Finished all meals\n", id);
    return 0;
}

int main() {
    printf("=== Dining Philosophers Problem ===\n");
    printf("Philosophers: %d\n", NUM_PHILOSOPHERS);
    printf("Meals per philosopher: %d\n", NUM_MEALS);
    printf("Strategy: Asymmetric solution (last philosopher picks in reverse)\n\n");

    // TODO: Create chopstick mutexes
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        chopsticksMutex[i] = CreateMutex(NULL, FALSE, NULL);
    }

    statsMutex = CreateMutex(NULL, FALSE, NULL);

    HANDLE philosophers[NUM_PHILOSOPHERS];

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers[i] = CreateThread(NULL, 0, Philosopher, (LPVOID)i, 0, NULL);
    }

    // Wait for all philosophers to finish
    WaitForMultipleObjects(NUM_PHILOSOPHERS, philosophers, TRUE, INFINITE);

    // Print statistics
    printf("\n=================================\n");
    printf("Final Statistics:\n");
    int totalMeals = 0;
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        printf("  Philosopher %d: %d meals\n", i, mealsEaten[i]);
        totalMeals += mealsEaten[i];
    }

    int expectedMeals = NUM_PHILOSOPHERS * NUM_MEALS;
    printf("\nTotal meals eaten: %d\n", totalMeals);
    printf("Expected meals: %d\n", expectedMeals);

    if (totalMeals == expectedMeals) {
        printf("\nSUCCESS: No deadlock! All philosophers ate their meals.\n");
    } else {
        printf("\nFAILED: Deadlock or starvation occurred!\n");
    }

    // TODO: Cleanup all handles
    CloseHandle(chopsticksMutex);
    CloseHandle(statsMutex);


    return 0;
}
