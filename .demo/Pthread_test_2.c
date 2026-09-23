/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pthread_test_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:47:14 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/23 21:47:17 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define NUM_THREADS 8

char	*join(const char *s1, const char *s2)
{
	char	*result;

	result = malloc(strlen(s1) + strlen(s2) + 1);
	if (result)
	{
		strcpy(result, s1);
		strcat(result, s2);
	}
	return (result);
}

struct					shared_data
{
	char				*firstletter;
	pthread_mutex_t		mutex;
};

struct					thread_data
{
	int					thread_id;
	int					sum;
	char				*message;
	struct shared_data	*shared;
};

struct thread_data		thread_data_array[NUM_THREADS];

void	*PrintHello(void *threadarg)
{
	struct thread_data	*my_data;
	int					taskid;
	int					sum;
	char				*hello_msg;
	char				*new_letters;

	my_data = threadarg;
	taskid = my_data->thread_id;
	sum = my_data->sum;
	hello_msg = my_data->message;
	sleep(1);
	printf("Thread %d: %s  Sum=%d\n", taskid, hello_msg, sum);
	pthread_mutex_lock(&my_data->shared->mutex);
	new_letters = join(my_data->shared->firstletter, hello_msg);
	my_data->shared->firstletter = new_letters;
	pthread_mutex_unlock(&my_data->shared->mutex);
	return (NULL);
}

int	main(int argc, char *argv[])
{
	char *messages[NUM_THREADS];
	pthread_t threads[NUM_THREADS];
	int *taskids[NUM_THREADS];
	struct shared_data data;
	int rc, t, sum;
	data.firstletter = "";
	pthread_mutex_init(&data.mutex, NULL);

	sum = 0;
	messages[0] = "English: Hello World!";
	messages[1] = "French: Bonjour, le monde!";
	messages[2] = "Spanish: Hola al mundo";
	messages[3] = "Klingon: Nuq neH!";
	messages[4] = "German: Guten Tag, Welt!";
	messages[5] = "Russian: Zdravstvuy, mir!";
	messages[6] = "Japan: Sekai e konnichiwa!";
	messages[7] = "Latin: Orbis, te saluto!";

	for (t = 0; t < NUM_THREADS; t++)
	{
		sum = sum + t;
		thread_data_array[t].thread_id = t;
		thread_data_array[t].sum = sum;
		thread_data_array[t].message = messages[t];
		thread_data_array[t].shared = &data;
		printf("Creating thread %d\n", t);
		rc = pthread_create(&threads[t], NULL, PrintHello,
				(void *)&thread_data_array[t]);
		if (rc)
		{
			printf("ERROR; return (code from pthread_create() is %d\n", rc));
			exit(-1);
		}
	}

	for (t = 0; t < NUM_THREADS; t++)
	{
		pthread_join(threads[t], NULL);
	}

	pthread_mutex_destroy(&data.mutex);

	printf("firste letter: %s\n", data.firstletter);
	pthread_exit(NULL);
}