/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dinner_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 19:36:30 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/12 16:30:11 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

// /**
//  * Initializes the monitor proccess, which verifies several semaphores linked
//  * to the program stability.
//  * @param table A pointer to the main enviroment philosopher structure.
//  * @note Once the monitor is correctly created, it's proccess will start
//  * the dinner.
//  */
// static void	create_monitor(t_rules *table)
// {
// 	table->pid_id[0] = fork();
// 	if (table->pid_id[0] < 0)
// 		forcend(table, seats, PH_PCS_CERR); //forcend should kill and wait for philo.
// 	if (!pid)
// 	{
// 		//create thread that blocks itself in sem_death
// 		//create thread that verifies meals_eaten
// 		//loop to start the dinner
// 		//waitpid for all philosopher proccess
// 		//exit self
// 	}
// }
	
// /**
//  * Initializes all necesary proccesses to run the philosopher program.
//  * The amount created is n_philo + 1 for the monitor.
//  * @param table A pointer to the main enviroment philosopher structure.
//  */
// int	start_dinner(t_rules *table)
// {
// 	t_philosopher	seat;
// 	int	pid;
// 	int	i;

// 	memset(&seat, 0, sizeof(t_philosopher));
// 	seat.table = table;
// 	seat.sem_meal = &(table->sem_meals[i]);
// 	i = -1;
// 	while (++i < table->n_philo)
// 	{
// 		seat.id = i + 1;
// 		seat.sem_name = sem_name(seat.id);
// 		table->pid_id[i + 1] = fork();
// 		if (pid < 0)
// 		{
// 			sem_post(table->sem_death);
// 			break;
// 		}
// 		if (!pid)
// 			return (/*philo(table, &seat)philosopher process*/);
// 		free(seat.sem_name);
// 	}
// 	create_monitor(table);
// 	waitpid(table->pid_id[0], NULL, 0);
// }
