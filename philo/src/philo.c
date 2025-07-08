/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 20:28:51 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/08 18:45:44 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	grab_forks(t_philosopher *seat)
{
	int	ms_death;

	if (!(seat->id % 2))
	{
		ms_death = seat->table->time_to_die;
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

void	eating(t_philosopher *seat)
{
	if (seat->table->n_philo == 1)
	{
		pthread_mutex_lock(seat->left_fork);
		to_print_access(seat, MTX_PRINT_FORK);
		pthread_mutex_unlock(seat->left_fork);
		usleep(seat->table->time_to_die * 2000);
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

void	*philo(void *arg)
{
	t_philosopher	*seat;
	int				ms_death;

	seat = (t_philosopher *) arg;
	ms_death = seat->table->time_to_die;
	while (to_death_flag(seat->table, MTX_FLAG_READ) < 0)
		usleep(100);
	to_time_value(seat, MTX_TIME_ISFULL);
	if ((seat->id % 2) != 0)
		usleep((seat->table->time_to_eat / 2) * 1000);
	while (!to_death_flag(seat->table, MTX_FLAG_READ))
	{
		to_print_access(seat, MTX_PRINT_THINK);
		if (!(seat->id % 2))
			usleep(200);
		eating(seat);
		to_print_access(seat, MTX_PRINT_SLEEP);
		usleep((seat->table->time_to_sleep * 1000));
	}
	return (NULL);
}
