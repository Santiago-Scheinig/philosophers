/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_routines.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:34:46 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 20:22:11 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

/**
 * A monitor local thread that waits on the death semaphore, ready to kill
 * every other child proccess if any of them dies.
 * 
 * @param arg A pointer to the main philosopher structure.
 * @return A pointer to an errcode if any.
 * @note Failures on this local thread are consider sensitive.
 */
void	*monitor_death(void *arg)
{
	t_rules *table;
	int		i;

	table = (t_rules *) arg;
	if (safe_sem_wait(table->sem_death, "/death", -1))
		THREAD_ERR(try_exit_and_kill(table, PH_SEM_WERR));
	if (try_exit_and_kill(table, PH_SUCCESS))
		THREAD_ERR(PH_PCS_KERR);
	i = -1;
	while (++i < table->n_philo)
		if (safe_sem_post(table->sem_ready, "/ready", -1))
			THREAD_ERR(PH_SEM_PERR);
	return (NULL);
}

/**
 * A monitor local thread that waits on the meals semaphore, ready to kill
 * every other child proccess once the max meals quota is accomplished.
 * 
 * @param arg A pointer to the main philosopher structure.
 * @return A pointer to an errcode if any.
 * @note Failures on this local thread are consider sensitive.
 */
void	*monitor_meals(void *arg)
{
	t_rules	*table;
	int		i;

	table = (t_rules *) arg;
	i = -1;
	if (safe_sem_wait(table->sem_start, "/start", -1))
		THREAD_ERR(try_exit_and_kill(table, PH_SEM_WERR));
	while (++i < table->n_philo)
		if (safe_sem_wait(table->sem_ready, "/ready", -1))
			THREAD_ERR(try_exit_and_kill(table, PH_SEM_WERR));
	if (try_exit_and_kill(table, PH_SUCCESS))
		THREAD_ERR(PH_PCS_KERR);
	if (safe_sem_post(table->sem_death, "/death", -1))
		THREAD_ERR(PH_SEM_PERR);
	return (NULL);
}

/**
 * A monitor local thread that waits on the ready semaphore, used to
 * wait for every proccess to be ready, before starting the experiment.
 * 
 * @param arg A pointer to the main philosopher structure.
 * @return A pointer to an errcode if any.
 * @note Failures on this local thread are consider sensitive.
 */
void	*monitor_start(void *arg)
{
	t_rules	*table;
	int		i;
	
	table = (t_rules *) arg;
	i = -1;
	while (++i < table->n_philo)
		if (safe_sem_wait(table->sem_ready, "/ready", -1))
			THREAD_ERR(try_exit_and_kill(table, PH_SEM_WERR));
	i = -1;
	while (++i < (table->n_philo + 1))
		if (safe_sem_post(table->sem_start, "/start", -1))
			THREAD_ERR(try_exit_and_kill(table, PH_SEM_PERR));
	return (NULL);
}

/**
 * A philosopher local thread used to verify [last_meal_time] and
 * [time_to_die] difference, setting the [sem_death] to 1 as well
 * as blocking the [sem_print] in order to prevent further writing
 * on STDIN and signal the monitor for it's own death.
 * 
 * @param arg A pointer to the main philosopher structure.
 * @return NULL since this thread it's detatched.
 */
void	*philo_death(void *arg)
{
	t_philosopher	*seat;
	struct timeval	day_time;
	struct timeval	last_meal;
	long			ms_last_meal;
	long			ms_day_time;
	
	seat = (t_philosopher *) arg;
	while (to_time_value(seat, SEM_TIME_IS).tv_sec == -1)
		usleep(1000);
	while (1)
	{
		gettimeofday(&day_time, NULL);
		last_meal = to_time_value(seat, SEM_TIME_IS);
		ms_last_meal = (last_meal.tv_sec * 1000L) + (last_meal.tv_usec / 1000);
		ms_day_time = (day_time.tv_sec * 1000L) + (day_time.tv_usec / 1000);
		if (seat->table->time_to_die < ms_day_time - ms_last_meal)
		{
			to_print_access(seat, SEM_PRINT_DEATH);
			return (NULL);
		}
		usleep(1000);
	}
}
