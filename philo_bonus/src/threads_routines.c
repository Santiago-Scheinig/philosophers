/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_routines.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:34:46 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/17 19:50:05 by sscheini         ###   ########.fr       */
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
	if (safe_sem_wait(table->sem_death, "/death"))
		THREAD_ERR(try_exit_and_kill(table, PH_SEM_WERR));
	if (try_exit_and_kill(table, PH_SUCCESS))
		THREAD_ERR(PH_PCS_KERR);
	i = -1;
	while (++i < table->n_philo)
		if (safe_sem_post(table->sem_ready, "/ready"))
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
	if (safe_sem_wait(table->sem_start, "/start"))
		THREAD_ERR(try_exit_and_kill(table, PH_SEM_WERR));
	while (++i < table->n_philo)
		if (safe_sem_wait(table->sem_ready, "/ready"))
			THREAD_ERR(try_exit_and_kill(table, PH_SEM_WERR));
	if (try_exit_and_kill(table, PH_SUCCESS))
		THREAD_ERR(PH_PCS_KERR);
	if (safe_sem_post(table->sem_death, "/death"))
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
		if (safe_sem_wait(table->sem_ready, "/ready"))
			THREAD_ERR(try_exit_and_kill(table, PH_SEM_WERR));
	i = -1;
	while (++i < (table->n_philo + 1))
		if (safe_sem_post(table->sem_start, "/start"))
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
	t_philosopher *seat;
	
	seat = (t_philosopher *) arg;
	while (1)
	{
		printf("Im cheking\n");
		usleep(10000);
	}
	return (NULL);
}