/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphores_actions.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:49:05 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/17 15:52:05 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"
#include <errno.h>

int	safe_sem_post(sem_t *sem, const char *sem_name)
{
	if (sem_post(sem) == -1)
	{
		printf("thd: sem_post failed on %s semaphore\n", sem_name);
		return (PH_SEM_PERR);
	}
	return (PH_SUCCESS);
}

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

// Can return FATAL_ERROR | PCS_KERR | SEM_WERR | SEM_PERR | SUCCESS
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
			if (kill(table->pid_id[i], SIGTERM))
			{
				error = 1;
				printf("thd: kill %i pid proccess failed\n", table->pid_id[i]);
			}
		}
		if (error)
			return (kill_all(table, 1));
	}
	if (safe_sem_post(table->sem_philo[0], "/philo_0"))
		return (kill_all(table, 1));
	return (errcode);
}
