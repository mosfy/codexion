/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init1.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:48:44 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/24 00:14:05 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	heap_destroy(t_heap *heap)
{
	if (heap && heap->tree)
	{
		free(heap->tree);
		heap->tree = NULL;
	}
	if (heap)
	{
		heap->size = 0;
		heap->capacity = 0;
	}
}

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
	if (strcmp(argv[7], "fifo") == 0)
		argv[7] = "0";
	else if (strcmp(argv[7], "edf") == 0)
		argv[7] = "1";
	else
		return (0);
	return (1);
}

void	init_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].condition_variable, NULL);
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
		sim->coders[i].last_compile_time = sim->start_timestamp;
		sim->coders[i].simulation = sim;
		sim->coders[i].l_dongle = &sim->dongles[i];
		sim->coders[i].r_dongle = &sim->dongles[(i + 1)
			% sim->number_of_coders];
		i++;
	}
}

t_heap_node	heap_pop(t_heap *heap)
{
	t_heap_node	empty_node;
	t_heap_node	root;

	empty_node = (t_heap_node){0, 0, 0};
	if (heap->size == 0)
		return (empty_node);
	root = heap->tree[0];
	heap->tree[0] = heap->tree[heap->size - 1];
	heap->size--;
	heap_sift_down(heap, 0);
	return (root);
}
