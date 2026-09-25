/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:48:44 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/25 21:10:22 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	parser(int argc, char *argv[])
{
	int	i;

	i = 1;
	if (argc != 9)
		return (-1);
	while (i != 7)
	{
		if (!ft_isdigit(argv[i]))
			return (-4);
		if (ft_atoi(argv[i]) <= 0)
			return (-2);
		i++;
	}
	if (strcmp(argv[8], "fifo") == 0)
		argv[8] = "0";
	else if (strcmp(argv[8], "edf") == 0)
		argv[8] = "1";
	else
		return (1);
	return (0);
}

void	init_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->dongles[i].is_in_use = 0;
		sim->dongles[i].cooldown_timestamp = 0;
		heap_init(&sim->dongles[i].queue, sim->number_of_coders,
			sim->scheduler_type);
		i++;
	}
}

void	init_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_time = 0;
		sim->coders[i].finished = 0;
		sim->coders[i].simulation = sim;
		sim->coders[i].l_dongle = &sim->dongles[i];
		sim->coders[i].r_dongle = &sim->dongles[(i + 1)
			% sim->number_of_coders];
		i++;
	}
}

