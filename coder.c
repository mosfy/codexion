/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 02:52:22 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/25 00:44:24 by tfrances         ###   ########.fr       */
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
		+ coder->simulation->dongle_cooldown;
	coder->r_dongle->is_in_use = 0;
	coder->r_dongle->cooldown_timestamp = get_time_in_ms()
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
	pthread_mutex_lock(&coder->simulation->mutex_stop);
	coder->last_compile_time = get_time_in_ms();
	pthread_mutex_unlock(&coder->simulation->mutex_stop);
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
		if (sim->number_of_compiles_required > 0
			&& coder->compile_count >= sim->number_of_compiles_required)
			return (NULL);
		coder_debug_and_refactor(coder);
	}
	return (NULL);
}

int	can_compile(t_coder *coder, long long now)
{
	t_dongle	*l;
	t_dongle	*r;

	l = coder->l_dongle;
	r = coder->r_dongle;
	now++;
	if (l->is_in_use || r->is_in_use)
		return (0);
	return (1);
}

void	take_dongles(t_coder *coder)
{
	t_heap_node	node;
	long long	now;

	node.coder_id = coder->id;
	node.arrival_time = get_time_in_ms();
	node.deadline = coder->last_compile_time
		+ coder->simulation->time_to_burnout;
	pthread_mutex_lock(&coder->simulation->mutex);
	printf("mutex lock\n");
	heap_push(&coder->l_dongle->queue, node);
	if (coder->l_dongle != coder->r_dongle)
		heap_push(&coder->r_dongle->queue, node);
	while (!is_simulation_stopped(coder->simulation))
	{
		now = get_time_in_ms();
		if (can_compile(coder, now))
		{
			break ;
		}
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
	printf("mutex unlock\n");
}
