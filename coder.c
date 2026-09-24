/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 02:52:22 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/24 22:29:12 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

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
		take_dongle(coder);
		coder_compile(coder);
		if (sim->number_of_compiles_required > 0
				&& coder->compile_count >= sim->number_of_compiles_required)
				return (NULL);
		coder_debug_and_refactor(coder);
	}
	return (NULL);
}

t_heap_node	create_coder_node(t_coder *coder)
{
	t_heap_node	node;

	node.coder_id = coder->id;
	node.arrival_time = get_time_in_ms();
	node.deadline = coder->last_compile_time
		+ coder->simulation->time_to_burnout;
	return (node);
}

void	acquire_single_dongle(t_coder *coder, t_dongle *dongle)
{
	t_heap_node	node;
	long long	now;

	node = create_coder_node(coder);
	printf("%s", node->)
	pthread_mutex_lock(&dongle->mutex_used);
	heap_push(&dongle->queue, node);
	while (!is_simulation_stopped(coder->simulation))
	{
		now = get_time_in_ms();
		if (dongle->queue.size > 0
			&& dongle->queue.tree[0].coder_id == coder->id)
		{
			if (dongle->is_in_use)
				pthread_cond_wait(&dongle->condition_variable,
					&dongle->mutex_used);
			else if (now < dongle->cooldown_timestamp)
			{
				pthread_mutex_unlock(&dongle->mutex_used);
				ft_usleep(dongle->cooldown_timestamp - now, coder->simulation);
				pthread_mutex_lock(&dongle->mutex_used);
			}
			else
				break ;
		}
		else
			pthread_cond_wait(&dongle->condition_variable, &dongle->mutex_used);
	}
	heap_pop(&dongle->queue);
	dongle->is_in_use = 1;
	pthread_mutex_unlock(&dongle->mutex_used);
}

void	take_dongle(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->l_dongle < coder->r_dongle)
	{
		first = coder->l_dongle;
		second = coder->r_dongle;
	}
	else
	{
		first = coder->r_dongle;
		second = coder->l_dongle;
	}
	acquire_single_dongle(coder, first);
	acquire_single_dongle(coder, second);
}

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
	pthread_mutex_lock(&coder->l_dongle->mutex_used);
	coder->l_dongle->is_in_use = 0;
	coder->l_dongle->cooldown_timestamp = get_time_in_ms()
		+ coder->simulation->dongle_cooldown;
	pthread_cond_broadcast(&coder->l_dongle->condition_variable);
	pthread_mutex_unlock(&coder->l_dongle->mutex_used);
	pthread_mutex_lock(&coder->r_dongle->mutex_used);
	coder->r_dongle->is_in_use = 0;
	coder->r_dongle->cooldown_timestamp = get_time_in_ms()
		+ coder->simulation->dongle_cooldown;
	pthread_cond_broadcast(&coder->r_dongle->condition_variable);
	pthread_mutex_unlock(&coder->r_dongle->mutex_used);
}

