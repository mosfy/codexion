/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 21:20:40 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/24 00:16:56 by tfrances         ###   ########.fr       */
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
	else
	{
		if (a.arrival_time < b.arrival_time)
			return (1);
		if (a.arrival_time > b.arrival_time)
			return (0);
		return (a.coder_id < b.coder_id);
	}
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

int	main(int argc, char *argv[])
{
	t_simulation	*simulation;

	if (parser(argc, argv) != 0)
		return (1);
	simulation = NULL;
	simulation_init(argv, simulation);
}
