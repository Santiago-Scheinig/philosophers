/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:52:48 by sscheini          #+#    #+#             */
/*   Updated: 2025/06/25 18:10:40 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/**
 * Success
 */
#define PH_SUCCESS	0
/**
 * Amount of arguments isn't valid
 */
#define PH_INVARGC	1
/**
 * Arguments aren't valid
 */
#define PH_INVARGV	2
/**
 * Mutex initialization failed
 */
#define PH_MUTEXIERR 3
/**
 * Mutex destruction failed"
 */
#define PH_MUTEXDERR 4
/**
 * Memory allocation failed
 */
#define PH_MEMFAIL 5

typedef struct s_rules
{
	int				n_philo;
	int				n_forks;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				meals_required;
	long long		start_time;
	int				death_flag;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	*forks;
}	t_rules;

typedef struct s_philosopher
{
	int				id;
	int				meals_eaten;
	long long		last_meal_time;
	pthread_t		thread_id;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	t_rules			rules;
}	t_philosopher;

/**
 * Finds the first number on a STRING with a decimal base.
 * @param str The string where the base number is saved.
 * @param base The base in which the number must be found.
 * @return The decimal INT found on STR.
 * @note A number can, but is not forced to, start with any
 * amount of spaces and one sign; but the next character must 
 * be a digit, if not or str doesn't exists, returns 0.
 */
int		ft_atoi(const char *nptr);

void	forcend(t_rules *table, int errmsg);