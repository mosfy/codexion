/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:51:13 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/24 01:24:55 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	simulation_init(char *argv[], t_simulation *sim)
{
	sim->number_of_coders = ft_atoi(argv[1]);
	sim->time_to_burnout = ft_atoi(argv[2]);
	sim->time_to_compile = ft_atoi(argv[3]);
	sim->time_to_debug = ft_atoi(argv[4]);
	sim->time_to_refactor = ft_atoi(argv[5]);
	sim->number_of_compiles_required = ft_atoi(argv[6]);
	sim->dongle_cooldown = ft_atoi(argv[7]);
	sim->scheduler_type = (strcmp(argv[8], "edf") == 0);
	sim->stop_flag = 0;
	sim->start_timestamp = get_time_in_miliseconde();
	pthread_mutex_init(sim->mutex_print, NULL);
	pthread_mutex_init(sim->mutex_stop, NULL);
	sim->coders = ft_calloc(sim->number_of_coders, sizeof(t_coder));
	sim->dongles = ft_calloc(sim->number_of_coders, sizeof(t_dongle));
	init_dongles(sim);
	init_coders(sim);
}

int	get_time_in_miliseconde(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec - tv.tv_usec);
}
