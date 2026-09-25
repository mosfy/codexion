/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfrances <tfrances@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 21:16:12 by tfrances          #+#    #+#             */
/*   Updated: 2026/09/25 22:21:50 by tfrances         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER
# define HEADER

# include <limits.h>
# include <pthread.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_simulation	t_simulation;

typedef struct s_heap_node
{
	int						coder_id;
	long long				arrival_time;
	long long				deadline;
}							t_heap_node;

typedef struct s_heap
{
	t_heap_node				*tree;
	int						size;
	int						capacity;
	int						scheduler;
}							t_heap;

typedef struct s_dongle
{
	long long				cooldown_timestamp;
	int						is_in_use;
	t_heap					queue;
}							t_dongle;

typedef struct s_coder
{
	int						id;
	pthread_t				thread;
	t_dongle				*l_dongle;
	t_dongle				*r_dongle;
	t_simulation			*simulation;
	long long				last_compile_time;
	int						compile_count;
	int						finished;
}							t_coder;

typedef struct s_simulation
{
	int						number_of_coders;
	long long				time_to_burnout;
	long long				time_to_compile;
	long long				time_to_debug;
	long long				time_to_refactor;
	int						number_of_compiles_required;
	long long				dongle_cooldown;
	int						stop_flag;
	int						scheduler_type; //fifo = 0 edf = 1
	long long				start_timestamp;
	pthread_mutex_t			mutex; // main mutex
	pthread_mutex_t			mutex_print;
	pthread_mutex_t			mutex_stop;
	pthread_mutex_t			mutex_time;
	pthread_cond_t			condition_variable;
	t_coder					*coders;
	t_dongle				*dongles;
	pthread_t				thread;
}							t_simulation;

/* Parsing & Simulation Initialization */
int							parser(int argc, char *argv[]);
void						simulation_init(char *argv[], t_simulation *sim);
void						init_dongles(t_simulation *sim);
void						init_coders(t_simulation *sim);
void						clean_simulation(t_simulation *sim);

/* Heap / Scheduler Operations */
void						heap_init(t_heap *heap, int capacity,
								int scheduler);
void						heap_push(t_heap *heap, t_heap_node node);
t_heap_node					heap_pop(t_heap *heap);
void						heap_sift_down(t_heap *heap, int i);
void						heap_destroy(t_heap *heap);
int							is_higher_priority(t_heap_node a, t_heap_node b,
								int scheduler);
void						pthread_monitor_init(t_simulation *simulation);

/* Coder Routine & Dongle Management */
void						*coder_tread(void *coder);
void						coder_compile(t_coder *coder);
void						take_dongles(t_coder *coder);
void						release_dongle(t_coder *coder);
int							is_simulation_stopped(t_simulation *sim);

/* Monitor Routine */
void						*monitor_tread(void *simulation);

/* Time & Output Utilities */
long long					get_time_in_ms(void);
void						ft_usleep(long long time_in_ms, t_simulation *sim);
void						print_status(t_coder *coder, char *status);
void						wait_10ms(t_simulation *sim);

/* Libft Helpers */
int							ft_atoi(const char *nbr);
int							ft_isdigit(char *c);
void						ft_bzero(void *s, size_t n);
void						*ft_calloc(size_t nmemb, size_t size);

#endif