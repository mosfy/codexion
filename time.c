/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:51:13 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/25 22:24:09 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)(tv.tv_sec * 1000) + (long long)(tv.tv_usec / 1000));
}

void	ft_usleep(long long time_in_ms, t_simulation *sim)
{
	long long	start;

	start = get_time_in_ms();
	while (!is_simulation_stopped(sim))
	{
		if (get_time_in_ms() - start >= time_in_ms)
			return ;
		usleep(500);
	}
}

void	wait_10ms(t_simulation *sim)
{
	struct timespec	ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += 10000000;
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&sim->condition_variable, &sim->mutex, &ts);
}
