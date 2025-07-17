/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads_routines.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:34:46 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/17 15:51:34 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

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

void	*philo_death(void *arg)
{
	t_philosopher *seat;
	
	seat = (t_philosopher *) arg;
	//CHECK DEATH
	return (NULL);
}