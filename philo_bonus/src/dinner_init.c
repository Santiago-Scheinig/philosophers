/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dinner_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 19:36:30 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/16 18:57:56 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

/**
 * Initializes the monitor proccess, which verifies several semaphores linked
 * to the program stability.
 * @param table A pointer to the main enviroment philosopher structure.
 * @note Once the monitor is correctly created, it's proccess will start
 * the dinner.
 */
static void	create_monitor(t_rules *table)
{
	pthread_t	thd_death_id;
	pthread_t	thd_meals_id;
	void		*ret;
	
	table->pid_id[0] = fork();
	if (table->pid_id[0] < 0)
		forcend(table, PH_PCS_CERR);
	if (!table->pid_id[0])
	{
		if (pthread_create(&(thd_death_id), NULL, monitor_death, table))
			exit(PH_THD_CERR);
		if (pthread_create(&(thd_meals_id), NULL, monitor_meals, table))
			exit(PH_THD_CERR);
		ret = NULL;
		if (pthread_join(thd_death_id, &ret) != 0)
			printf("Error: Unable to join monitor_death thread.\n");//Exit?
		if (ret)
			exit((__intptr_t)ret);
		ret = NULL;	
		if (pthread_join(thd_meals_id, &ret) != 0)
			printf("Error: Unable to join monitor_meals thread.\n");//Exit?
		if (ret)
			exit((__intptr_t)ret);
		exit(PH_SUCCESS);
	}
}	

static void	create_philosophers(t_rules *table)
{
	t_philosopher	seat;
	int				i;

	memset(&seat, 0, sizeof(t_philosopher));
	seat.table = table;
	i = -1;
	while (++i < table->n_philo)
	{
		seat.id = i + 1;
		seat.sem_name = table->sem_names[seat.id];
		seat.sem_philo = table->sem_philo[seat.id];
		table->pid_id[seat.id] = fork();
		if (table->pid_id[seat.id] < 0)
		{
			sem_post(table->sem_death);
			break;
		}
		if (!table->pid_id[seat.id])
			philosophize(table, &seat);
	}
}

/**
 * Initializes all necesary proccesses to run the philosopher program.
 * The amount created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 */
void	initialize_dinner(t_rules *table)
{
	int	i;

	create_monitor(table);
	create_philosophers(table);
	/* 	i = -1; Must use /ready sem
	while (++i < table.n_philo)
		sem_post(table.sem_start); */
	i = 0;
	while (++i < table->n_philo)
		if (waitpid(table->pid_id[i], NULL, 0) < 0)
			printf("Error: Unable to wait on pid %i", table->pid_id[i]);
}
