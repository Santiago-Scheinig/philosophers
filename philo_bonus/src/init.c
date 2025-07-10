/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 19:36:30 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/10 21:02:15 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


char	*sem_name(int nbr)
{
	static int	i = -1;
	const char	*base_name = "/philo_";
	char		*new_name;

	new_name = malloc(18 * sizeof(char));
	if (!new_name)
		return (NULL);
	memset(new_name, 0, (18 * sizeof(char)));
	while (base_name[++i])
		new_name = base_name[i];
	while (nbr >= 10)
	{
		new_name = (nbr % 10) + 48;
		nbr /= 10;
	}
	new_name = (nbr + 48);
	return (new_name);
}

/**
 * Close every semaphore created to run the philosopher program.
 * @param table A pointer to the main enviroment philosopher structure.
 * @note If any semaphore close fails, an error message is
 * printed on screen detailing the failed semaphore, the program
 * then exits with forcend(4).
 */
void	close_semaphores(t_rules *table)
{
	static int	i = -1;
	char		*name;
	int			ans;

	ans = 0;
	if (sem_close(table->sem_death) != 0 || sem_unlink("/death") != 0)
		ans += printf("Error: Unable to close %s semaphore\n", "/death");
	if (sem_close(table->sem_print) != 0 || sem_unlink("/print") != 0)
		ans += printf("Error: Unable to close %s semaphore\n", "/print");
	if (sem_close(table->sem_forks) != 0 || sem_unlink("/forks") != 0)
		ans += printf("Error: Unable to close %s semaphore\n", "/print");
	while (++i < table->n_forks)
	{
		name = sem_name(i + 1);
		if (!name)
			forcend(table, NULL, PH_MEM_NERR);
		if (sem_close(&(table->sem_meals[i])) != 0 || sem_unlink(name) != 0)
			ans += printf("Error: Unable to close semaphore /philo_%d\n", i);
		free(name);
	}
	if (table->sem_meals)
		free(table->sem_meals);
	if (ans)
		forcend(table, PH_SEM_DERR);
}

/**
 * Initializes all necessary semaphores to run the philosopher program.
 * 
 * 	- Death semaphore: Used to avoid data races with the death flag.
 *
 * 	- Print semaphore: Used to avoid data races with the print flag.
 * 
 * 	- Forks semaphore: Used to follow remaining forks.
 * 
 *	- Meals semaphore: Used to avoid data races with the meals flag.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 */
void	initialize_semaphore(t_rules *table, int n_philo)
{
	static int	i = -1;
	char		*name;

	table->sem_death = sem_open("/death", O_CREAT | O_EXCL, 0644, 0);
	if (table->sem_death == SEM_FAILED)
		forcend(NULL, PH_SEM_IERR);
	table->sem_print = sem_open("/print", O_CREAT | O_EXCL, 0644, 1);
	if (table->sem_print == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	table->sem_forks = sem_open("/forks", O_CREAT | O_EXCL, 0644, n_philo / 2);
	if (table->sem_forks == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	table->sem_start = sem_open("/start", O_CREAT | O_EXCL, 0644, 0);
	if (table->sem_start == SEM_FAILED)
		forcend(table, PG_SEM_IERR);
	while (++i < table->n_philo)
	{
		name = sem_name(i + 1);
		if (!name)
			forcend(table, PH_MEM_NERR);
		table->sem_meals[i] = sem_open(name, O_CREAT | O_EXCL, 0644, 1);
		free(name);
		if (table->sem_meals[i] == SEM_FAILED)
			forcend(table, PH_SEM_IERR);
	}
}

/**
 * Initializes the monitor proccess, which verifies several semaphores linked
 * to the program stability.
 * @param table A pointer to the main enviroment philosopher structure.
 * @note Once the monitor is correctly created, it's proccess will start
 * the dinner.
 */
static void	create_monitor(t_rules *table)
{
	table->pid_id[0] = fork();
	if (pid < 0)
		forcend(table, seats, PH_THD_CERR); //forcend should kill and wait for philo.
	if (!pid)
	{
		//create thread that blocks itself in sem_death
		//create thread that verifies meals_eaten
		//sem_wait(sem_death) - that way it wont start the waitpid loop until end of dinner.
		//waitpid for all philosopher proccess
		//exit self
	}
	//loop to start the dinner
}
	
/**
 * Initializes all necesary proccesses to run the philosopher program.
 * The amount created is n_philo + 1 for the monitor.
 * @param table A pointer to the main enviroment philosopher structure.
 */
int	start_dinner(t_rules *table)
{
	t_philosopher	seat;
	int	pid;
	int	i;

	memset(&seat, 0, sizeof(t_philosopher));
	seat.table = table;
	seat.sem_meal = &(table->sem_meals[i]);
	i = -1;
	while (++i < table->n_philo)
	{
		seat.id = i + 1;
		table->pid_id[i + 1] = fork();
		if (pid < 0)
		{
			sem_post(&(table->sem_death));
			break;
		}
		if (!pid)
			return (/*philo(table, &seat)philosopher process*/);
	}
	create_monitor(table);
	//waitpid monitor til i < table.n_philo || pid[i] = -1
}
