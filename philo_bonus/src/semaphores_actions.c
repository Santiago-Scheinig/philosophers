/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphores_actions.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 18:47:45 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 19:42:28 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

struct timeval to_time_value(t_philosopher *seat, t_sem_time action)
{
	struct timeval	val;

	gettimeofday(&val, NULL);
	if (safe_sem_wait(seat->sem_philo, "/philo", seat->id))
		exit(PH_SEM_WERR);
	if (action == SEM_TIME_ISEATING)
		seat->is_eating = 1;
	if (action == SEM_TIME_ISFULL)
	{
		seat->is_eating = 0;
		seat->last_meal_time = val;
	}
	if (action == SEM_TIME_IS && !seat->is_eating)
		val = seat->last_meal_time;
	if (safe_sem_post(seat->sem_philo, "/philo", seat->id))
		exit(PH_SEM_WERR);
	return (val);
}

static long	get_time_of_dinner(struct timeval start_time)
{
	struct timeval	tv;
	long			ms_start;
	long			ms_tv;

	gettimeofday(&tv, NULL);
	ms_tv = (tv.tv_sec * 1000L) + (tv.tv_usec / 1000);
	ms_start = (start_time.tv_sec * 1000L) + (start_time.tv_usec / 1000);
	return (ms_tv - ms_start);
}

static void print_death_status(t_philosopher *seat, long ms_tv)
{
	if (safe_sem_post(seat->table->sem_death, "/death", -1))
		exit(PH_SEM_PERR);
	printf("%06lims %i died.\n", ms_tv, seat->id);
	exit(PH_SUCCESS);
}

void	to_print_access(t_philosopher *seat, t_sem_print action)
{
	long ms_tv;

	ms_tv = get_time_of_dinner(seat->start_time);
	if (safe_sem_wait(seat->table->sem_print, "/print", -1))
		exit(PH_SEM_WERR);
	usleep(100);
	if (action == SEM_PRINT_DEATH)
		print_death_status(seat, ms_tv);
	else if (action == SEM_PRINT_EAT)
		printf("%06lims %i is eating.\n", ms_tv, seat->id);
	else if (action == SEM_PRINT_FORK)
	{
		printf("%06lims %i has taken a fork.\n", ms_tv, seat->id);
		if (seat->table->n_philo > 1)
			printf("%06lims %i has taken a fork.\n", ms_tv, seat->id);
	}
	else if (action == SEM_PRINT_SLEEP)
		printf("%06lims %i is sleeping.\n", ms_tv, seat->id);
	else if (action == SEM_PRINT_THINK)
		printf("%06lims %i is thinking.\n", ms_tv, seat->id);
	if (safe_sem_post(seat->table->sem_print, "/print", -1))
		exit(PH_SEM_PERR);
}
