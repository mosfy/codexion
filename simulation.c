/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 03:46:47 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/24 07:21:47 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	clean_simulation(t_simulation *sim)
{
	int	i;

	if (!sim)
		return ;
	if (sim->dongles)
	{
		i = 0;
		while (i < sim->number_of_coders)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex_time);
			pthread_mutex_destroy(&sim->dongles[i].mutex_used);
			pthread_cond_destroy(&sim->dongles[i].condition_variable);
			heap_destroy(&sim->dongles[i].queue);
			i++;
		}
		free(sim->dongles);
	}
	if (sim->coders)
		free(sim->coders);
	pthread_mutex_destroy(&sim->mutex_print);
	pthread_mutex_destroy(&sim->mutex_stop);
	free(sim);
}

void	simulation_init(char *argv[], t_simulation *sim)
{
	sim->number_of_coders = ft_atoi(argv[1]);
	sim->time_to_burnout = ft_atoi(argv[2]);
	sim->time_to_compile = ft_atoi(argv[3]);
	sim->time_to_debug = ft_atoi(argv[4]);
	sim->time_to_refactor = ft_atoi(argv[5]);
	sim->number_of_compiles_required = ft_atoi(argv[6]);
	sim->dongle_cooldown = ft_atoi(argv[7]);
	sim->scheduler_type = ft_atoi(argv[8]);
	sim->stop_flag = 0;
	sim->start_timestamp = get_time_in_ms();
	pthread_mutex_init(&sim->mutex_print, NULL);
	pthread_mutex_init(&sim->mutex_stop, NULL);
	sim->coders = ft_calloc(sim->number_of_coders, sizeof(t_coder));
	sim->dongles = ft_calloc(sim->number_of_coders, sizeof(t_dongle));
	if (!sim->coders || !sim->dongles)
		return ;
	init_dongles(sim);
	init_coders(sim);
}