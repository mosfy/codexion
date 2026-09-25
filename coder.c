/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 02:52:22 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/25 23:38:13 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	is_simulation_stopped(t_simulation *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->mutex_stop);
	stopped = sim->stop_flag;
	pthread_mutex_unlock(&sim->mutex_stop);
	return (stopped);
}

void	print_status(t_coder *coder, char *status)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->simulation->mutex_print);
	if (!is_simulation_stopped(coder->simulation))
	{
		timestamp = get_time_in_ms() - coder->simulation->start_timestamp;
		printf("%lld %d %s\n", timestamp, coder->id, status);
	}
	pthread_mutex_unlock(&coder->simulation->mutex_print);
}

void	release_dongle(t_coder *coder)
{
	pthread_mutex_lock(&coder->simulation->mutex);
	coder->l_dongle->is_in_use = 0;
	coder->l_dongle->cooldown_timestamp = get_time_in_ms()
		- coder->simulation->start_timestamp
		+ coder->simulation->dongle_cooldown;
	coder->r_dongle->is_in_use = 0;
	coder->r_dongle->cooldown_timestamp = get_time_in_ms()
		- coder->simulation->start_timestamp
		+ coder->simulation->dongle_cooldown;
	pthread_cond_broadcast(&coder->simulation->condition_variable);
	pthread_mutex_unlock(&coder->simulation->mutex);
}

void	coder_debug_and_refactor(t_coder *coder)
{
	print_status(coder, "is debugging");
	ft_usleep(coder->simulation->time_to_debug, coder->simulation);
	print_status(coder, "is refactoring");
	ft_usleep(coder->simulation->time_to_refactor, coder->simulation);
}

void	coder_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->simulation->mutex_time);
	coder->last_compile_time = get_time_in_ms()
		- coder->simulation->start_timestamp;
	pthread_mutex_unlock(&coder->simulation->mutex_time);
	print_status(coder, "is compiling");
	ft_usleep(coder->simulation->time_to_compile, coder->simulation);
	release_dongle(coder);
	coder->compile_count++;
}

void	*coder_tread(void *thread_coder)
{
	t_coder			*coder;
	t_simulation	*sim;

	coder = (t_coder *)thread_coder;
	sim = coder->simulation;
	while (!is_simulation_stopped(sim))
	{
		take_dongles(coder);
		if (is_simulation_stopped(sim))
			break ;
		coder_compile(coder);
		coder_debug_and_refactor(coder);
		if (sim->number_of_compiles_required > 0
			&& coder->compile_count >= sim->number_of_compiles_required)
		{
			pthread_mutex_lock(&sim->mutex_time);
			coder->finished = 1;
			pthread_mutex_unlock(&sim->mutex_time);
			return (NULL);
		}
	}
	return (NULL);
}

int	is_first_in_queue(t_heap *heap, int coder_id)
{
	if (heap->size == 0)
		return (0);
	return (heap->tree[0].coder_id == coder_id);
}

void	print_heap(t_heap *heap)
{
	int	i;

	i = 0;
	printf("[ ");
	while (i < heap->size)
	{
		printf("%d ", heap->tree[i].coder_id);
		i++;
	}
	printf("]\n");
}

int	can_compile(t_coder *coder)
{
	t_dongle	*l;
	t_dongle	*r;
	long long	l_time;
	long long	r_time;
	long long	now;

	now = get_time_in_ms();
	l = coder->l_dongle;
	r = coder->r_dongle;
	now -= coder->simulation->start_timestamp;
	l_time = l->cooldown_timestamp;
	r_time = r->cooldown_timestamp;
	if (l->is_in_use || r->is_in_use)
		return (0);
	if (l->cooldown_timestamp > now || r->cooldown_timestamp > now)
		return (0);
	//if (!is_first_in_queue(&l->queue, coder->id))
	//	return (0);
	//if (!is_first_in_queue(&r->queue, coder->id))
	//	return (0);
	return (1);
}

// first created a node with node information than you lock everything that need
// to be lock and you try to take a dongle and you see if you can !!! than delock cause
// your a kind person
void	take_dongles(t_coder *coder)
{
	t_heap_node	node;

	node.coder_id = coder->id;
	node.arrival_time = get_time_in_ms();
	pthread_mutex_lock(&coder->simulation->mutex);
	pthread_mutex_lock(&coder->simulation->mutex_time);
	node.deadline = coder->last_compile_time
		+ coder->simulation->time_to_burnout;
	pthread_mutex_unlock(&coder->simulation->mutex_time);
	heap_push(&coder->l_dongle->queue, node);
	if (coder->l_dongle != coder->r_dongle)
		heap_push(&coder->r_dongle->queue, node);
	while (!is_simulation_stopped(coder->simulation) && !can_compile(coder))
	{
		wait_10ms(coder->simulation);
	}
	if (!is_simulation_stopped(coder->simulation))
	{
		heap_pop(&coder->l_dongle->queue);
		if (coder->l_dongle != coder->r_dongle)
			heap_pop(&coder->r_dongle->queue);
		coder->l_dongle->is_in_use = 1;
		coder->r_dongle->is_in_use = 1;
	}
	pthread_mutex_unlock(&coder->simulation->mutex);
}
