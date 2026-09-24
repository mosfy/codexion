/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:51:13 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/24 23:03:06 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL));
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
