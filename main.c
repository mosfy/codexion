/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 21:20:40 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/25 22:28:47 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	is_higher_priority(t_heap_node a, t_heap_node b, int scheduler)
{
	if (scheduler == 1)
	{
		if (a.deadline < b.deadline)
			return (1);
		if (a.deadline > b.deadline)
			return (0);
		return (a.coder_id < b.coder_id);
	}
	if (a.arrival_time < b.arrival_time)
		return (1);
	if (a.arrival_time > b.arrival_time)
		return (0);
	return (a.coder_id < b.coder_id);
}

void	heap_init(t_heap *heap, int capacity, int scheduler)
{
	heap->tree = ft_calloc(capacity, sizeof(t_heap_node));
	heap->scheduler = scheduler;
	heap->capacity = capacity;
	heap->size = 0;
}

void	heap_push(t_heap *heap, t_heap_node node)
{
	int			i;
	int			parent;
	t_heap_node	tmp;

	if (heap->size >= heap->capacity)
		return ;
	i = heap->size;
	heap->tree[i] = node;
	heap->size++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (is_higher_priority(heap->tree[i], heap->tree[parent],
				heap->scheduler))
		{
			tmp = heap->tree[i];
			heap->tree[i] = heap->tree[parent];
			heap->tree[parent] = tmp;
			i = parent;
		}
		else
			break ;
	}
}

void	heap_sift_down(t_heap *heap, int i)
{
	int			left;
	int			right;
	int			highest;
	t_heap_node	tmp;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		highest = i;
		if (left < heap->size && is_higher_priority(heap->tree[left],
				heap->tree[highest], heap->scheduler))
			highest = left;
		if (right < heap->size && is_higher_priority(heap->tree[right],
				heap->tree[highest], heap->scheduler))
			highest = right;
		if (highest == i)
			break ;
		tmp = heap->tree[i];
		heap->tree[i] = heap->tree[highest];
		heap->tree[highest] = tmp;
		i = highest;
	}
}

void	set_stop(t_simulation *sim)
{
	pthread_mutex_lock(&sim->mutex_stop);
	sim->stop_flag = 1;
	pthread_mutex_unlock(&sim->mutex_stop);
}

void	*monitor_thread(void *simulation)
{
	int				i;
	int				j;
	long long		last;
	long long		now;
	t_simulation	*sim;

	sim = (t_simulation *)simulation;
	while (!is_simulation_stopped(sim))
	{
		now = get_time_in_ms() - sim->start_timestamp;
		i = 0;
		j = 0;
		while (i < sim->number_of_coders)
		{
			pthread_mutex_lock(&sim->mutex_time);
			if (sim->coders[i].finished)
			{
				pthread_mutex_unlock(&sim->mutex_time);
				i++;
				j++;
				continue ;
			}
			last = sim->coders[i].last_compile_time;
			pthread_mutex_unlock(&sim->mutex_time);

			if (now - last >= sim->time_to_burnout)
			{
				set_stop(sim);

				pthread_mutex_lock(&sim->mutex_print);
				printf("%lld %d burned out\n",
					now, sim->coders[i].id);
				pthread_mutex_unlock(&sim->mutex_print);

				pthread_mutex_lock(&sim->mutex);
				pthread_cond_broadcast(&sim->condition_variable);
				pthread_mutex_unlock(&sim->mutex);

				return (NULL);
			}
			i++;
		}
		if (j == sim->number_of_coders)
		{
			set_stop(sim);
		}
		usleep(1000);
	}
	return (NULL);
}

void	pthread_monitor_init(t_simulation *simulation)
{
	int	rc;

	rc = pthread_create(&simulation->thread, NULL, monitor_thread,
			(void *)simulation);
	if (rc)
	{
		printf("ERROR; return (code from pthread_create() is %d\n", rc);
		exit(-1);
	}
}

int	main(int argc, char *argv[])
{
	t_simulation	*simulation;
	int				rc;
	int				i;

	if (parser(argc, argv) != 0)
		return (1);
	simulation = malloc(sizeof(t_simulation));
	if (!simulation)
		return (1);
	simulation_init(argv, simulation);
	i = 0;
	while (i < simulation->number_of_coders)
	{
		rc = pthread_create(&simulation->coders[i].thread, NULL, coder_tread,
				(void *)&simulation->coders[i]);
		if (rc)
		{
			printf("ERROR; return (code from pthread_create() is %d\n", rc);
			exit(-1);
		}
		i++;
	}
	pthread_monitor_init(simulation);
	i = 0;
	while (i < simulation->number_of_coders)
	{
		pthread_join(simulation->coders[i].thread, NULL);
		i++;
	}
	pthread_join(simulation->thread, NULL);
	free(simulation);
	return (0);
}
