/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 21:16:12 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/21 06:57:05 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>


typedef struct s_simulation t_simulation;

typedef struct s_dongle
{
    pthread_mutex_t mutex;
    pthread_cond_t condition_variable;
    long long cooldown_timestamp;
}t_dongle;

typedef struct s_coder
{
    int id;
    pthread_t thread;
    t_dongle *l_dongle;
    t_dongle *r_dongle;
    t_simulation *simulation;
    int last_compile_time;
    int compile_count;
}t_coder;

struct s_simulation
{
    int number_of_coders;
    long long time_to_burnout;
    long long time_to_compile;
    long long time_to_debug;
    long long time_to_refactor;
    int number_of_compiles_required;
    long long dongle_cooldown;
    int stop_flag;
    int scheduler_type;
    long long start_timestamp;
    pthread_mutex_t mutex_print;
    pthread_mutex_t mutex_stop;
    t_coder **coders;
    t_dongle **dongles;
};

typedef struct s_heap_node
{
    int         coder_id;
    long long   arrival_time;
    long long   deadline;
} t_heap_node;

typedef struct s_heap
{
    t_heap_node *tree;
    int         size;
    int         capacity;
    int         scheduler;
} t_heap;


int		main(int argc, char *argv[]);
int		ft_atoi(const char *nbr);
int		ft_isdigit(char *c);
void	ft_bzero(void *s, size_t n);
void	*ft_calloc(size_t nmemb, size_t size);