/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtx_actions.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 17:11:22 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 20:44:14 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * Using the T_MTX_FLAG enum, executes instructions on the death_flag
 * variable, saved on the main experiment structure, avoiding data races using
 * a mutex.
 * 
 * @param table A pointer to the main experiment structure.
 * @param action The action to perform into death_flag.
 * @return The current value of deat_flag.
 */
int	to_death_flag(t_rules *table, t_mtx_flag action)
{
	int	val;

	val = -1;
	pthread_mutex_lock(&(table->death_mutex));
	if (action == MTX_FLAG_READ)
		val = table->death_flag;
	else if (action == MTX_FLAG_OFF)
	{
		table->death_flag = 0;
		val = table->death_flag;
	}
	else if (action == MTX_FLAG_ON)
	{
		table->death_flag = 1;
		val = table->death_flag;
	}
	else if (action == MTX_FLAG_INC)
		val = ++(table->death_flag);
	else if (action == MTX_FLAG_SUB)
		val = --(table->death_flag);
	pthread_mutex_unlock(&(table->death_mutex));
	return (val);
}

/**
 * Using the T_MTX_FLAG enum, executes instructions on the meals_eaten
 * variable, saved on the T_PHILOSOPHER structure, avoiding data races using
 * a mutex.
 * 
 * @param seat A pointer to a T_PHILOSOPHER structure linked to the meals_eaten
 * variable.
 * @param action The action to perfrom into meals_eaten
 * @return The current value of meals_eaten on seat.
 */
int	to_meals_value(t_philosopher *seat, t_mtx_flag action)
{
	int	val;

	val = -1;
	pthread_mutex_lock(seat->meal_mutex);
	if (action == MTX_FLAG_READ)
		val = seat->meals_eaten;
	else if (action == MTX_FLAG_OFF)
	{
		seat->meals_eaten = 0;
		val = seat->meals_eaten;
	}
	else if (action == MTX_FLAG_ON)
	{
		seat->meals_eaten = 1;
		val = seat->meals_eaten;
	}
	else if (action == MTX_FLAG_INC)
		val = ++(seat->meals_eaten);
	else if (action == MTX_FLAG_SUB)
		val = --(seat->meals_eaten);
	pthread_mutex_unlock(seat->meal_mutex);
	return (val);
}

/**
 * Using the T_MTX_TIME enum, executes instructions on the last_meal timestamp
 * variable, saved on the T_PHILOSOPHER structure, avoiding data races using
 * a mutex.
 * 
 * @param seat A pointer to a T_PHILOSOPHER structure linked to the last_meal
 * timestamp variable.
 * @param action The action to perfrom into last_meal_time.
 * @return If is_eating is true, returns the current timeval. Else, returns 
 * the last meal timeval on seat.
 */
struct timeval	to_time_value(t_philosopher *seat, t_mtx_time action)
{
	struct timeval	val;

	gettimeofday(&val, NULL);
	pthread_mutex_lock(seat->meal_mutex);
	if (action == MTX_TIME_ISEATING)
		seat->is_eating = 1;
	if (action == MTX_TIME_ISFULL)
	{
		seat->is_eating = 0;
		seat->last_meal_time = val;
	}
	if (action == MTX_TIME_IS && !seat->is_eating)
		val = seat->last_meal_time;
	pthread_mutex_unlock(seat->meal_mutex);
	return (val);
}

/**
 * Returns the current time of the dinner, using a start_time, in ms.
 * 
 * @param start_time The time in which the dinner started.
 * @return The current time in ms. 
*/
static int	get_time_of_dinner(struct timeval start_time)
{
	struct timeval	tv;
	long			ms_start;
	long			ms_tv;

	gettimeofday(&tv, NULL);
	ms_tv = (tv.tv_sec * 1000L) + (tv.tv_usec / 1000);
	ms_start = (start_time.tv_sec * 1000L) + (start_time.tv_usec / 1000);
	return (ms_tv - ms_start);
}

/**
 * Using the T_MTX_PRINT enum, prints a specific message on STDOUT linked to
 * the pointer to a T_PHILOSOPHER seat, avoiding print chunks using a mutex.
 * 
 * @param seat A pointer to the T_PHILOSOPHER linked to the message.
 * @param action The action that specifies the message to print.
 */
void	to_print_access(t_philosopher *seat, t_mtx_print action)
{
	long	ms_tv;

	ms_tv = get_time_of_dinner(seat->table->start_time);
	pthread_mutex_lock(&(seat->table->print_mutex));
	if (!to_death_flag(seat->table, MTX_FLAG_READ))
	{
		if (action == MTX_PRINT_DEATH)
		{
			to_death_flag(seat->table, MTX_FLAG_ON);
			printf("%06lims %i died.\n", ms_tv, seat->id);
		}
		else if (action == MTX_PRINT_EAT)
			printf("%06lims %i is eating.\n", ms_tv, seat->id);
		else if (action == MTX_PRINT_FORK)
		{
			printf("%06lims %i has taken a fork.\n", ms_tv, seat->id);
			if (seat->table->n_philo > 1)
				printf("%06lims %i has taken a fork.\n", ms_tv, seat->id);
		}
		else if (action == MTX_PRINT_SLEEP)
			printf("%06lims %i is sleeping.\n", ms_tv, seat->id);
		else if (action == MTX_PRINT_THINK)
			printf("%06lims %i is thinking.\n", ms_tv, seat->id);
	}
	pthread_mutex_unlock(&(seat->table->print_mutex));
}
