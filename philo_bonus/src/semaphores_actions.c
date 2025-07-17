/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphores_actions.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:49:05 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/17 19:43:22 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"
#include <errno.h>

/**
 * Failsafe for sem_post().
 * 
 * @param sem A pointer to the semaphore to post.
 * @param sem_name The name of the semaphore to post.
 * @return 0 on success. On error, the specific errcode.
 * @note If sem_post() fails, an error log is printed on STDOUT.
 */
int	safe_sem_post(sem_t *sem, const char *sem_name)
{
	if (sem_post(sem) == -1)
	{
		printf("thd: sem_post failed on %s semaphore\n", sem_name);
		return (PH_SEM_PERR);
	}
	return (PH_SUCCESS);
}

/**
 * Failsafe for sem_wait().
 * 
 * @param sem A pointer to the semaphore to wait.
 * @param sem_name The name of the semaphore to wait.
 * @return 0 on success. On error, the specific errcode.
 * @note If sem_wait() fails, an error log is printed on STDOUT.
 */
int	safe_sem_wait(sem_t *sem, const char *sem_name)
{
	while (sem_wait(sem) == -1)
	{
		if (errno != EINTR)
		{
			printf("thd: sem_wait failed on %s semaphore\n", sem_name);
			return (PH_SEM_WERR);
		}
	}
	return (PH_SUCCESS);
}

/**
 * Failsafe to try_exit_and_kill().
 * 
 * Kills all child proccessess with SEGKILL instead.
 * 
 * @param table A pointer to the main philosopher structure.
 * @param index The position of the last failed kill pid.
 * @return An exit errcode.
 * @note If SEGKILL fails, it will lastly try to kill all child proccesses,
 * including the current one executing it. Failing the last one is consider
 * a FATAL error.
 */
static int	kill_all(t_rules *table, int index)
{
	int error;

	error = 0;
	while (index < table->n_philo)
	{
		if (kill(table->pid_id[index], SIGKILL) == -1)
			error = 1;
		index++;
	}
	if (error)
	{
		if (kill(0, SIGKILL) == -1)
		{
			printf("FATAL: Unable to kill proccesses. Force exit.\n");
			return (PH_FATAL_ERROR);
		}
	}
	return (PH_PCS_KERR);
}

/**
 * Cleanly kills all child proccesses with SEGTERM.
 * 
 * @param table A pointer to the main philosopher structure.
 * @param errcode The errcode that triggered the exit and kill proccess.
 * @return The errcode that triggered the exit and kill proccess.
 * @note Any error occured during this execution it's consider sensitive, and
 * will execute kill_all() as a countermeasure.
 */
int try_exit_and_kill(t_rules *table, int errcode)
{
	int	error;
	int	i;

	if (safe_sem_wait(table->sem_philo[0], "/philo_0"))
		return (kill_all(table, 1));
	if (table->exit_flag == 0)
	{
		table->exit_flag = 1;
		i = 0;
		error = 0;
		while (table->pid_id[++i] != -1)
		{
			if (kill(table->pid_id[i], SIGTERM) == -1)
			{
				error = 1;
				printf("thd: kill %i pid proccess failed\n", i);
				break;
			}
		}
		if (error)
			return (kill_all(table, i));
	}
	if (safe_sem_post(table->sem_philo[0], "/philo_0"))
		return (kill_all(table, 1));
	return (errcode);
}
