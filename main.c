/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 21:20:40 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/21 07:52:41 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

static int is_higher_priority(t_heap_node a, t_heap_node b, int scheduler)
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

void heap_init(t_heap *heap, int capacity, int scheduler)
{
	heap->tree = ft_calloc(capacity, sizeof(t_heap_node));
	heap->scheduler = scheduler;
	heap->capacity = capacity;
	heap->size = 0;
}

void heap_push(t_heap *heap, t_heap_node node)
{
	int i;
	int parent;

	if (heap->size >= heap->capacity)
		return ;

    i = heap->size;
    heap->tree[i] = node;
    heap->size++;

    while (i > 0)
    {
        parent = (i - 1) / 2;
        if (is_higher_priority(heap->tree[i], heap->tree[parent], heap->scheduler))
        {
            t_heap_node tmp = heap->tree[i];
            heap->tree[i] = heap->tree[parent];
            heap->tree[parent] = tmp;
            i = parent;
        }
        else
            break ;
    }
}

t_heap_node heap_pop(t_heap *heap)
{
    t_heap_node empty_node = {0, 0, 0};
    
    if (heap->size == 0)
        return (empty_node);

    t_heap_node root = heap->tree[0];

    heap->tree[0] = heap->tree[heap->size - 1];
    heap->size--;

    int i = 0;
    while (1)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int highest = i;

        if (left < heap->size && is_higher_priority(heap->tree[left], heap->tree[highest], heap->scheduler))
            highest = left;
        if (right < heap->size && is_higher_priority(heap->tree[right], heap->tree[highest], heap->scheduler))
            highest = right;

        if (highest != i)
        {
            t_heap_node tmp = heap->tree[i];
            heap->tree[i] = heap->tree[highest];
            heap->tree[highest] = tmp;
            i = highest;
        }
        else
            break ;
    }

    return (root);
}

void heap_destroy(t_heap *heap)
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
	if (argc != 8)
		return (-1);
	while (i != 7)
	{
		if (!ft_isdigit(argv[i]))
			return(-4);
		if (ft_atoi(argv[i]) < 0)
			return (-2);
		i++;
	}
	if (strcmp (argv[7], "fifo") != 0 && (strcmp (argv[7], "edf") != 0))
		return (-3);
	return (0);
}

int	main(int argc, char *argv[])
{
	if (parser(argc, argv) != 0)
		return (1);
}
