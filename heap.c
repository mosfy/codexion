/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 03:48:26 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/24 03:52:44 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

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

void	heap_destroy(t_heap *heap)
{
	if (!heap)
		return ;
	if (heap->tree)
	{
		free(heap->tree);
		heap->tree = NULL;
	}
}
