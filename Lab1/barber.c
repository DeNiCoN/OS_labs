#include <windows.h>
#include <process.h>
#include <time.h>
#include <stdio.h>

#define	BUF_SIZE	10
#define NUM_THREADS 7


DWORD WINAPI Barber(LPVOID param);
DWORD WINAPI Consumer(LPVOID param);

HANDLE mutex;
HANDLE new_customer;
HANDLE next_customer;
HANDLE release_customer;

int	next = -1;

int main()
{
	DWORD	ThreadId;
	HANDLE	threads[2];
	int	params[] = { 0, 1, 2, 3, 4, 5, 6};

    mutex = CreateSemaphore(0, 1, 1, 0);
    new_customer = CreateEvent(NULL, FALSE, FALSE, NULL);
    next_customer = CreateEvent(NULL, FALSE, FALSE, NULL);
    release_customer = CreateEvent(NULL, FALSE, FALSE, NULL);

    for (int i = 0; i < 1; i++)
    {
        threads[i] = CreateThread(0, 0, Barber, &params[i], 0, &ThreadId);
    }

    for (int i = 1; i < NUM_THREADS; i++)
    {
        threads[i] = CreateThread(0, 0, Consumer, &params[i], 0, &ThreadId);
    }


	for (int i = 0; i < NUM_THREADS; i++)
		WaitForSingleObject(threads[i], INFINITE);
}

DWORD WINAPI Barber(LPVOID param)
{
	int	id = * (DWORD *)param;

	srand(time(0) + id);

	for (;;)
	{
		WaitForSingleObject(mutex, INFINITE);
        if (next == -1)
        {
            printf("Barber sleep\n");

            ReleaseSemaphore(mutex, 1, 0);
            SetEvent(next_customer);
            WaitForSingleObject(new_customer, INFINITE);
            printf("Barber wake\n");
        }
        else
        {
            printf("Barber working with customer %d\n", next);
            int	prod = rand() % 1000;
            Sleep(prod);
            printf("Barber finished %d\n", next);
            next = -1;

            ReleaseSemaphore(mutex, 1, 0);
            SetEvent(release_customer);
        }
	}

	return 0;
}



DWORD WINAPI Consumer(LPVOID param)
{
	int	id = * (DWORD *)param;

	srand(time(0) + id);

	for (;;)
	{
		int	consume;
		Sleep(rand() % 9000);

        printf("Customer %d stands in queue\n", id);

        WaitForSingleObject(next_customer, INFINITE);

		WaitForSingleObject(mutex, INFINITE);
        next = id;
        printf("Customer %d sit\n", id);
        ReleaseSemaphore(mutex, 1, 0);
        SetEvent(new_customer);
        WaitForSingleObject(release_customer, INFINITE);
	}

	return 0;
}

