/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 17:16:20 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/10 16:34:35 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	cronometer(struct timeval last_meal, long ms_death)
{
	struct timeval	day_time;
	long			ms_last_meal;
	long			ms_day_time;

	gettimeofday(&day_time, NULL);
	ms_last_meal = (last_meal.tv_sec * 1000L) + (last_meal.tv_usec / 1000);
	ms_day_time = (day_time.tv_sec * 1000L) + (day_time.tv_usec / 1000);
	if (ms_death <= ms_day_time - ms_last_meal)
		return (1);
	return (0);
}

static int	check_dinner_status(t_monitor *waiter)
{
	struct timeval	last_meal;
	int meals_required;
	int	ans;
	int	i;

	i = -1;
	ans = 0;
	meals_required = waiter->table->meals_required;
	if (waiter->table->n_philo == 1)
		usleep(waiter->table->time_to_die * 1000);
	while (++i < waiter->table->n_philo)
	{
		last_meal = to_time_value(&(waiter->seats[i]), MTX_TIME_IS);
 		if (cronometer(last_meal, waiter->table->time_to_die))
		{
			to_print_access(&(waiter->seats[i]), MTX_PRINT_DEATH);
			return (0);
		}
		if (to_meals_value(&(waiter->seats[i]), MTX_FLAG_READ) < meals_required)
			ans++;
	}
	if (!ans && meals_required > 0)
		return (0);
	return (1);
}

void	*monitorize(void *arg)
{
	t_monitor	*waiter;
	int			i;

	waiter = (t_monitor *) arg;
	gettimeofday(&(waiter->table->start_time), NULL);
	to_death_flag(waiter->table, MTX_FLAG_OFF);
	while (!to_death_flag(waiter->table, MTX_FLAG_READ))
	{
		usleep(500);
		if (!check_dinner_status(waiter))
			to_death_flag(waiter->table, MTX_FLAG_ON);
	}
	i = -1;
	while (++i < waiter->table->n_philo)
		if (pthread_join(waiter->seats[i].thd_id, NULL) != 0)
			printf("Error: Unable to join philosopher thread %d\n", i);
	free(waiter);
	return (NULL);
}