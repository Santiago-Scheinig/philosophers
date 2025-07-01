/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mtx_actions.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 17:11:22 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/01 20:38:45 by sscheini         ###   ########.fr       */
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

/**
 * Using the T_MTX_PRINT enum, prints a specific message on STDOUT linked to
 * the pointer to a T_PHILOSOPHER seat, avoiding print chunks using a mutex.
 * 
 * @param seat A pointer to the T_PHILOSOPHER linked to the message.
 * @param action The action to perform into death_flag.
 */
void	to_print_access(t_philosopher *seat, t_mtx_print action)
{
	pthread_mutex_lock(&(seat->table->print_mutex));
	if (action == MTX_PRINT_EAT)
		printf("%i is eating.\n", seat->id);
	//All executions needed to print the correct msgs.
	pthread_mutex_unlock(&(seat->table->print_mutex));
}
