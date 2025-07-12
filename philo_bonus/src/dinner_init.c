/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dinner_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 19:36:30 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/12 17:54:38 by sscheini         ###   ########.fr       */
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
	int			i;
	
	table->pid_id[0] = fork();
	if (table->pid_id[0] < 0)
		forcend(table, PH_PCS_CERR);
	if (!table->pid_id[0])
	{
		if (pthread_create(&(thd_death_id), NULL, monitor_death, table))
			forcend(table, PH_THD_CERR);
		pthread_detach(thd_death_id);
		if (pthread_create(&(thd_meals_id), NULL, monitor_meals, table))
			forcend(table, PH_THD_CERR);
		pthread_detach(thd_meals_id);
		i = 0;
		while (++i < table->n_philo)
			if (waitpid(table->pid_id[i], NULL, 0) < 0)
				printf("Error: Unable to wait on pid %i", table->pid_id[i]);
		exit(PH_SUCCESS);
	}
}
	
/**
 * Initializes all necesary proccesses to run the philosopher program.
 * The amount created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 */
int	initialize_dinner(t_rules *table)
{
	t_philosopher	seat;
	int	pid;
	int	i;

	create_monitor(table);
	memset(&seat, 0, sizeof(t_philosopher));
	seat.table = table;
	seat.sem_meal = &(table->sem_meals[i]);
	i = -1;
	while (++i < table->n_philo)
	{
		seat.id = i + 1;
		seat.sem_name = table->sem_names[seat.id];
		table->pid_id[seat.id] = fork();
		if (pid < 0)
		{
			sem_post(table->sem_death);
			break;
		}
		if (!pid)
			return (philosophize(table, &seat));
	}
}
