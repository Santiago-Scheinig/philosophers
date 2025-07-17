/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_proccess.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:49:01 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/17 15:52:37 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

void	philosophize(t_rules *table, t_philosopher *seat)
{
	if (table && seat)
		printf("hello!");
	//create the philo_death thread
	//signal sem_post(ready)
	//sem_wait(start) waiting for the main to start the dinner
	//philosopher proccess to think, eat and sleep until death or meals_to_eat
}