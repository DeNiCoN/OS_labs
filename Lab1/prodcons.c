
#include <windows.h>
#include <process.h>
#include <time.h>
#include <stdio.h>

#define	BUF_SIZE	10
#define NUM_THREADS 7


DWORD WINAPI Producer(LPVOID param);
DWORD WINAPI Consumer(LPVOID param);

HANDLE mutex;
HANDLE not_full;
HANDLE not_empty;

int	buffer[BUF_SIZE];
int size = 0;
int	front = 0;
int	back = 0;

int main()
{
	DWORD	ThreadId;
	HANDLE	threads[2];
	int	params[] = { 0, 1, 2, 3, 4, 5, 6};

    mutex = CreateSemaphore(0, 1, 1, 0);
    not_full = CreateEvent(NULL, FALSE, FALSE, NULL);
    not_empty = CreateEvent(NULL, FALSE, FALSE, NULL);

    for (int i = 0; i < 3; i++)
    {
        threads[i] = CreateThread(0, 0, Producer, &params[i], 0, &ThreadId);
    }

    for (int i = 3; i < NUM_THREADS; i++)
    {
        threads[i] = CreateThread(0, 0, Consumer, &params[i], 0, &ThreadId);
    }


	for (int i = 0; i < NUM_THREADS; i++)
		WaitForSingleObject(threads[i], INFINITE);
}

DWORD WINAPI Producer(LPVOID param)
{
	int	id = * (DWORD *)param;

	srand(time(0) + id);

	for (;;)
	{
		int	prod = rand() % 3000;
		Sleep(prod);

		WaitForSingleObject(mutex, INFINITE);
        if (size != BUF_SIZE)
        {
            buffer[front] = prod;
            front = (front + 1) % BUF_SIZE;
            size++;
            printf("%d producer %d %d\n", size, id, prod);

            ReleaseSemaphore(mutex, 1, 0);
            SetEvent(not_empty);
        }
        else
        {
            printf("producer %d sleep\n", id);
            ReleaseSemaphore(mutex, 1, 0);
            WaitForSingleObject(not_full, INFINITE);
            printf("producer %d wake\n", id);
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
		Sleep(rand() % 2000);

		WaitForSingleObject(mutex, INFINITE);

        if (size != 0)
        {
            consume = buffer[back];
            back = (back + 1) % BUF_SIZE;
            size--;

            printf("%d consumer %d %d\n", size, id, consume);
            ReleaseSemaphore(mutex, 1, 0);
            SetEvent(not_full);
        }
        else
        {
            printf("consumer %d sleep\n", id);
            ReleaseSemaphore(mutex, 1, 0);
            WaitForSingleObject(not_empty, INFINITE);
            printf("consumer %d wake\n", id);
        }

	}

	return 0;
}

