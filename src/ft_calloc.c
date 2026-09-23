/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 21:46:45 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/23 21:46:47 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../header.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	unsigned char	*tmp;

	if (size != 0 && nmemb > SIZE_MAX / size)
		return (NULL);
	tmp = malloc(nmemb * size);
	if (!tmp)
		return (NULL);
	ft_bzero (tmp, nmemb * size);
	return (tmp);
}