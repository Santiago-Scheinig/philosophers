/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 20:28:51 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/01 20:37:54 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
void	sleeping()
{
	usleep
}

void	eating()
{
	checks if it can use forks
	if cant, then thinks
}

void	thinking()
{
	uses his resources to philosophate. maybe even print some funny quotes.
	check if starvating
}

void	starving()
{
	checks if the philosopher has starved
	if has, sets the death_flag to 1, prints death, and exits self
	if hasnt, goes back to thinking
}
*/

static int	check_meals_eaten(t_monitor *waiter)
{
	int meals_required;
	int	ans;
	int	i;

	i = -1;
	ans = 0;
	meals_required = waiter->table->meals_required;
	while (++i < waiter->table->n_philo)
		if (to_meals_value(&(waiter->seats[i]), MTX_FLAG_READ) < meals_required)
			ans++;
	if (!ans)
		return (0);
	return (1);
}

void	*monitorize(void *arg)
{
	t_monitor	*waiter;

	waiter = (t_monitor *) arg;
	to_death_flag(waiter->table, MTX_FLAG_OFF);
	while (!to_death_flag(waiter->table, MTX_FLAG_READ))
		if (!check_meals_eaten(waiter))
			to_death_flag(waiter->table, MTX_FLAG_ON);
	return (NULL);
}

void	*philosophizing(void *arg)
{
	t_philosopher	*seat;

	seat = (t_philosopher *) arg;
	while (to_death_flag(seat->table, MTX_FLAG_READ) < 0)
		usleep(100);
	while (!to_death_flag(seat->table, MTX_FLAG_READ))
	{
		usleep(seat->table->time_to_eat);
		to_print_access(seat, MTX_PRINT_EAT);
		to_meals_value(seat, MTX_FLAG_INC);
		usleep(seat->table->time_to_sleep);
		//try to_print_access();
		/* eating();
		[
			thinking();while is waiting for the monitor to allow him to take his forks
				starving();check regularry if it has starved, if not, uses its resources to think
			taking_forks();lock left and right fork
			leaving_forks();unlock left and right fork
		]
		sleeping(); */
	}
	return (NULL);
}
