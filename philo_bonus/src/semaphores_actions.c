/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphores_actions.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:49:05 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/16 18:28:33 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"
#include <errno.h>

int	safe_sem_post(sem_t *sem, const char *sem_name)
{
	if (sem_wait(sem) == -1)
	{
		printf("ERROR: sem_post failed on %s semaphore.\n", sem_name);
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
			printf("ERROR: sem_wait failed on %s semaphore.\n", sem_name);
			return (PH_SEM_WERR);
		}
	}
	return (PH_SUCCESS);
}

void	try_exit_and_kill(t_rules *table)
{
	safe_sem_wait(table->sem_philo[0]);
	if (table->exit_flag == 0)
	{
		table->exit_flag = 1;
		kill_all_philosophers(table);
	}
	safe_sem_post(table->sem_philo[0]);
}