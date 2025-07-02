/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtx_actions.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 17:11:22 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/02 18:12:33 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * Using the t_mtx_flag enum, executes instructions on the death_flag
 * variable, saved on the main experiment structure, avoiding data races using
 * a mutex.
 * 
 * @param table A pointer to the main experiment structure.
 * @param action The action to perform into death_flag.
 */
int	to_death_flag(t_rules *table, t_mtx_flag action)
{
	int val;

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
 * Using the t_mtx_flag enum, executes instructions on the meals_eaten
 * variable, saved on the T_PHILOSOPHER structure, avoiding data races using
 * a mutex.
 * 
 * @param seat A pointer to a T_PHILOSOPHER structure linked to the meals_eaten
 * variable.
 * @param action The action to perfrom into meals_eaten
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
 * @param action The action to perform into death_flag.
 */
void	to_print_access(t_philosopher *seat, t_mtx_print action)
{
	long	ms_tv;
	int		death_status;

	death_status = to_death_flag(seat->table, MTX_FLAG_READ);
	ms_tv = get_time_of_dinner(seat->table->start_time);
	pthread_mutex_lock(&(seat->table->print_mutex));
	if (!death_status)
	{
		if (action == MTX_PRINT_DEATH)
		{
			to_death_flag(seat->table, MTX_FLAG_ON);
			printf("%010lims %i died.\n", ms_tv, seat->id);
		}
		else if (action == MTX_PRINT_EAT)
			printf("%010lims %i is eating.\n", ms_tv, seat->id);
		else if (action == MTX_PRINT_FORK)
			printf("%010lims %i has taken a fork.\n", ms_tv, seat->id);
		else if (action == MTX_PRINT_SLEEP)
			printf("%010lims %i is sleeping.\n", ms_tv, seat->id);
		else if (action == MTX_PRINT_THINK)
			printf("%010lims %i is thinking.\n", ms_tv, seat->id);
	}
	pthread_mutex_unlock(&(seat->table->print_mutex));
}
