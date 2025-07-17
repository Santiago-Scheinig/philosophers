/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_proccess.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:49:01 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/17 18:54:26 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

void	philosophize(t_rules *table, t_philosopher *seat)
{
	if (safe_sem_post(table->sem_ready, "/ready"))
		exit(PH_SEM_PERR);
	if (safe_sem_wait(table->sem_start, "/start"))
		exit(PH_SEM_WERR);
	while (table && seat)
	{
		usleep(1000000);
		if (safe_sem_post(table->sem_ready, "/ready"))
			exit(PH_SEM_PERR);
	}
	//create the philo_death thread
	//signal sem_post(ready)
	//sem_wait(start) waiting for the main to start the dinner
	//philosopher proccess to think, eat and sleep until death or meals_to_eat
}