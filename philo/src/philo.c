/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 20:28:51 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 20:46:47 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
 * Indicates the philosopher thread to grab forks using a mutex lock.
 * 
 * @param seat A pointer to the T_PHILOSOPHER thread structure.
 */
static void	grab_forks(t_philosopher *seat)
{
	if (!(seat->id % 2))
	{
		pthread_mutex_lock(seat->left_fork);
		pthread_mutex_lock(seat->right_fork);
		to_print_access(seat, MTX_PRINT_FORK);
	}
	else
	{
		pthread_mutex_lock(seat->right_fork);
		pthread_mutex_lock(seat->left_fork);
		to_print_access(seat, MTX_PRINT_FORK);
	}
}

/**
 * Indicates the philosopher thread to start eating:
 * 
 * - Locks the forks it needs.
 * 
 * - Sets [is_eating] flag to 1.
 * 
 * - Executes an usleep equal to [time_to_eat].
 * 
 * - Increments the [meals_eaten] value by one.
 * 
 * - Unlocks the forks it used.
 * 
 * - Updates [last_meal_time].
 * 
 * @param seat A pointer to the T_PHILOSOPHER thread structure.
 * @note If [n_philo] equals 1, then a specific execution is made, where
 * the thread grabs just one fork and waits until [time_to_die].
 */
static void	eating(t_philosopher *seat)
{
	if (seat->table->n_philo == 1)
	{
		pthread_mutex_lock(seat->left_fork);
		to_print_access(seat, MTX_PRINT_FORK);
		pthread_mutex_unlock(seat->left_fork);
		usleep(seat->table->time_to_die * 1100);
	}
	else
	{
		grab_forks(seat);
		to_time_value(seat, MTX_TIME_ISEATING);
		to_print_access(seat, MTX_PRINT_EAT);
		usleep((seat->table->time_to_eat * 1000));
		to_meals_value(seat, MTX_FLAG_INC);
		pthread_mutex_unlock(seat->left_fork);
		pthread_mutex_unlock(seat->right_fork);
		to_time_value(seat, MTX_TIME_ISFULL);
	}
}

/**
 * In case of odd [n_philosopher] amount, it verifies if the thread needs
 * to eat or not. To do so, compares the current time agains the [last_meal]
 * time, and verifies it against ([time_to_die] / 2).
 * 
 * @param seat A pointer to the T_PHILOSOPHER thread structure.
 * @return Returns 0 if it's less than ([time_to_die] / 2). Else, returns 1.
 */
static int	is_hungry(t_philosopher *seat)
{
	struct timeval	tv;
	struct timeval	last_meal;
	int				ms_tv;
	int				ms_last_meal;

	gettimeofday(&tv, NULL);
	ms_tv = (tv.tv_sec * 1000L) + (tv.tv_usec / 1000);
	last_meal = to_time_value(seat, MTX_TIME_IS);
	ms_last_meal = (last_meal.tv_sec * 1000L) + (last_meal.tv_usec / 1000);
	if (ms_tv - ms_last_meal <= (seat->table->time_to_die / 2))
		return (0);
	return (1);
}

/**
 * The philosopher thread routine.
 * 
 * - Sets the [last_meal_time] to [start_time].
 * 
 * - If odd, waits ([time_to_eat] / 2) to create eating waves.
 * 
 * - While [death_flag] is false: Thinks, eats and sleeps.
 * 
 * @param arg A pointer to the needed arguments of the routine. A pointer to
 * the T_PHILOSOPHER structure in this case.
 * @note If [n_philosopher] it's an odd amount, uses is_hungry() to set 
 * priorities between waves.
 */
void	*philo(void *arg)
{
	t_philosopher	*seat;

	seat = (t_philosopher *) arg;
	while (to_death_flag(seat->table, MTX_FLAG_READ) < 0)
		usleep(100);
	to_time_value(seat, MTX_TIME_ISFULL);
	if ((seat->id % 2) != 0 && seat->table->n_philo != 1)
		usleep((seat->table->time_to_eat / 2) * 1000);
	while (!to_death_flag(seat->table, MTX_FLAG_READ))
	{
		to_print_access(seat, MTX_PRINT_THINK);
		eating(seat);
		if (to_death_flag(seat->table, MTX_FLAG_READ))
			return (NULL);
		to_print_access(seat, MTX_PRINT_SLEEP);
		usleep((seat->table->time_to_sleep * 1000));
		if ((seat->table->n_philo % 2))
			while (!is_hungry(seat))
				usleep(100);
	}
	return (NULL);
}
