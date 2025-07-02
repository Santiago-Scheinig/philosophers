/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 20:28:51 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/02 18:25:48 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	leave_forks(t_philosopher *seat)
{
	if (!(seat->id % 2))
	{
		pthread_mutex_unlock(seat->left_fork);
		pthread_mutex_unlock(seat->right_fork);
	}
	else
	{
		pthread_mutex_unlock(seat->right_fork);
		pthread_mutex_unlock(seat->left_fork);
	}
}

void	grab_forks(t_philosopher *seat)
{
	if (!(seat->id % 2))
	{
		pthread_mutex_lock(seat->left_fork);
		to_print_access(seat, MTX_PRINT_FORK);
		pthread_mutex_lock(seat->right_fork);
		to_print_access(seat, MTX_PRINT_FORK);
	}
	else
	{
		pthread_mutex_lock(seat->right_fork);
		to_print_access(seat, MTX_PRINT_FORK);
		pthread_mutex_lock(seat->left_fork);
		to_print_access(seat, MTX_PRINT_FORK);
	}
}

void	eating(t_philosopher *seat)
{
	if (seat->table->n_philo == 1)
	{
		pthread_mutex_lock(seat->left_fork);
		to_print_access(seat, MTX_PRINT_FORK);
		pthread_mutex_unlock(seat->left_fork);
		usleep(seat->table->time_to_die * 1000);
		to_print_access(seat, MTX_PRINT_DEATH);
	}
	else
	{
		grab_forks(seat);
		memset(&(seat->last_meal_time), 0, sizeof(struct timeval));
		to_print_access(seat, MTX_PRINT_EAT);
		usleep((seat->table->time_to_eat * 1000));
		to_meals_value(seat, MTX_FLAG_INC);
		leave_forks(seat);
		gettimeofday(&(seat->last_meal_time), NULL);
	}
}

void	*philo(void *arg)
{
	t_philosopher	*seat;

	seat = (t_philosopher *) arg;
	while (to_death_flag(seat->table, MTX_FLAG_READ) < 0)
		usleep(100);
	while (!to_death_flag(seat->table, MTX_FLAG_READ))
	{
		gettimeofday(&(seat->last_meal_time), NULL);
		to_print_access(seat, MTX_PRINT_THINK);
		eating(seat);
		to_print_access(seat, MTX_PRINT_SLEEP);
		usleep((seat->table->time_to_sleep * 1000));
	}
	return (NULL);
}
