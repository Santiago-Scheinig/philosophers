/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_proccess.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:49:01 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 19:41:38 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

static void	eating(t_philosopher *seat)
{
	if (seat->table->n_philo == 1)
	{
		to_print_access(seat, SEM_PRINT_FORK);
		usleep(seat->table->time_to_die * 1100);
	}
	if (safe_sem_wait(seat->table->sem_forks, "/forks", -1))
		exit(PH_SEM_WERR);
	to_print_access(seat, SEM_PRINT_FORK);
	to_time_value(seat, SEM_TIME_ISEATING);
	to_print_access(seat, SEM_PRINT_EAT);
	usleep((seat->table->time_to_eat * 1000));
	if (seat->table->meals_required)
	{
		if (++seat->meals_eaten >= seat->table->meals_required 
		&& !seat->max_meals_achieved)
		{
			if (safe_sem_post(seat->table->sem_ready, "/ready", -1))
				exit(PH_SEM_PERR);
			seat->max_meals_achieved = 1;
		}
	}
	if (safe_sem_post(seat->table->sem_forks, "/forks", -1))
		exit(PH_SEM_PERR);
	to_time_value(seat, SEM_TIME_ISFULL);
}

static int	is_hungry(t_philosopher *seat)
{
	struct timeval	tv;
	struct timeval	last_meal;
	int				ms_tv;
	int				ms_last_meal;

	gettimeofday(&tv, NULL);
	ms_tv = (tv.tv_sec * 1000L) + (tv.tv_usec / 1000);
	last_meal = to_time_value(seat, SEM_TIME_IS);
	ms_last_meal = (last_meal.tv_sec * 1000L) + (last_meal.tv_usec / 1000);
	if (ms_tv - ms_last_meal <= (seat->table->time_to_die / 2))
		return (0);
	return (1);
}

static void	philo_init(t_philosopher *seat)
{
	pthread_t	thd_starved_id;

	seat->last_meal_time.tv_sec = -1;
	seat->last_meal_time.tv_usec = -1;
	if (pthread_create(&(thd_starved_id), NULL, philo_death, seat))
		exit(PH_THD_CERR);
	pthread_detach(thd_starved_id);
	if (safe_sem_post(seat->table->sem_ready, "/ready", -1))
		exit(PH_SEM_PERR);
	if (safe_sem_wait(seat->table->sem_start, "/start", -1))
		exit(PH_SEM_WERR);
	to_time_value(seat, SEM_TIME_ISFULL);
	gettimeofday(&(seat->start_time), NULL);
	to_print_access(seat, SEM_PRINT_THINK);
	if ((seat->id % 2) != 0 && seat->table->n_philo != 1)
		usleep((seat->table->time_to_eat / 2) * 1000);
}

void	philosophize(t_philosopher *seat)
{
	philo_init(seat);
	while (1)
	{
		eating(seat);
		to_print_access(seat, SEM_PRINT_SLEEP);
		usleep((seat->table->time_to_sleep * 1000));
		if ((seat->table->n_philo % 2))
			while (!is_hungry(seat))
				usleep(100);
		to_print_access(seat, SEM_PRINT_THINK);
	}
}
