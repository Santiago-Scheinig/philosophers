/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 17:16:20 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/02 18:16:31 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	cronometer(struct timeval start, struct timeval last, long limit)
{
	long			ms_last;
	long			ms_start;

	if (!last.tv_sec && !last.tv_usec)
		return (0);
	ms_last = (last.tv_sec * 1000L) + (last.tv_usec / 1000);
	ms_start = (start.tv_sec * 1000L) + (start.tv_usec / 1000);
	if (limit <= ms_last - ms_start)
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
	while (++i < waiter->table->n_philo)
	{
		last_meal = waiter->seats[i].last_meal_time;
 		if (cronometer(waiter->table->start_time, last_meal, waiter->table->time_to_die))
		{
			to_print_access(&(waiter->seats[i]), MTX_PRINT_DEATH);
			return (0);
		}
		if (to_meals_value(&(waiter->seats[i]), MTX_FLAG_READ) < meals_required)
			ans++;
	}
	if (!ans && meals_required)
		return (0);
	return (1);
}

void	*monitorize(void *arg)
{
	t_monitor	*waiter;
	int			i;

	waiter = (t_monitor *) arg;
	to_death_flag(waiter->table, MTX_FLAG_OFF);
	gettimeofday(&(waiter->table->start_time), NULL);
	while (!to_death_flag(waiter->table, MTX_FLAG_READ))
	{
		if (!check_dinner_status(waiter))
			to_death_flag(waiter->table, MTX_FLAG_ON);
		usleep(100);
	}
	i = -1;
	while (++i < waiter->table->n_philo)
		if (pthread_join(waiter->seats[i].thd_id, NULL) != 0)
			printf("Error: Unable to join philosopher thread %d\n", i);
	free(waiter);
	return (NULL);
}