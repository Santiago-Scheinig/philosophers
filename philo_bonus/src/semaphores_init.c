/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphores_init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 15:23:49 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/12 16:44:58 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

static char	*get_sem_name(char *name, int name_len, int id)
{
	while (id >= 10)
	{
		name[name_len] = (id % 10) + 48;
		id /= 10;
		name_len++;
	}
	name[name_len] = (id + 48);
	return (name);
}

/**
 * Initializes all necessary STRINGS to initialize philosophers semaphores.
 * 
 * The base of the semaphore name is "/philo_[id]". Reserving the [id] = 0
 * for the monitor semaphore.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * 
 * @note The id corresponding each philosopher proccess is inverted.
 */
void	initialize_sem_names(t_rules *table)
{
	const char	*base_name = "/philo_";
	static int	i = -1;
	int			j;

	table->sem_names = malloc((table->n_philo + 2) * sizeof(char *));
	if (!table->sem_names)
		forcend(table, PH_MEM_AERR);
	table->sem_names[table->n_philo + 1] = NULL;
	while(++i < table->n_philo + 1)
	{
		j = -1;
		table->sem_names[i] = malloc(20 * sizeof(char));
		if (!table->sem_names[i])
		{
			while (--i >= 0)
				free(table->sem_names[i]);
			forcend(table, PH_MEM_AERR);
		}
		memset(table->sem_names[i], 0, (20 * sizeof(char)));
		while (base_name[++j])
			table->sem_names[i][j] = base_name[j];
		table->sem_names[i] = get_sem_name(table->sem_names[i], 7, i);
	}
}

/**
 * Unlinks every semaphore created to run the philosopher program.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * 
 * @note If any semaphore unlink fails, an error message is
 * printed on screen detailing the failed semaphore, the program
 * then exits with forcend(4).
 */
void	unlink_semaphores(t_rules *table)
{
	char	*name;
	int		i;
	int		ans;

	ans = 0;
	if (sem_unlink("/death"))
		ans += printf("Error: Unable to unlink /death semaphore\n");
	if (sem_unlink("/print"))
		ans += printf("Error: Unable to unlink /print semaphore\n");
	if (sem_unlink("/forks"))
		ans += printf("Error: Unable to unlink /forks semaphore\n");
	if (sem_unlink("/start"))
		ans += printf("Error: Unable to unlink /start semaphore\n");
	i = -1;
	while (++i < table->n_philo + 1)
	{
		name = table->sem_names[i];
		if (sem_unlink(name))
			ans += printf("Error: Unable to unlink semaphore /philo_%d\n", i);
	}
	if (ans)
		forcend(table, PH_SEM_UERR);
}

/**
 * Close every semaphore created to run the philosopher program.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 * 
 * @note If any semaphore close fails, an error message is
 * printed on screen detailing the failed semaphore, the program
 * then exits with forcend(5).
 */
void	close_semaphores(t_rules *table)
{
	int	i;
	int	ans;

	ans = 0;
	if (sem_close(table->sem_death))
		ans += printf("Error: Unable to close /death semaphore\n");
	if (sem_close(table->sem_print))
		ans += printf("Error: Unable to close /print semaphore\n");
	if (sem_close(table->sem_forks))
		ans += printf("Error: Unable to close /forks semaphore\n");
	if (sem_close(table->sem_start))
		ans += printf("Error: Unable to close /start semaphore\n");
	i = -1;
	while (++i < table->n_philo + 1)
		if (sem_close(table->sem_philo[i]))
			ans += printf("Error: Unable to close semaphore /philo_%d\n", i);
	if (table->sem_philo)
	{
		free(table->sem_philo);
		table->sem_philo = NULL;
	}
	if (ans)
		forcend(table, PH_SEM_DERR);
}

/**
 * Initializes all necessary semaphores to run the philosopher program.
 * 
 * 	- Death semaphore: Used to follow-up proccesses death.
 *
 * 	- Print semaphore: Used to avoid write interleaving between proccesses.
 * 
 * 	- Forks semaphore: Used to follow-up remaining forks.
 * 
 *	- Start semaphore: Used to link the start of philosopher proccesses.
 * 
 *	- Philo semaphore: Used to avoid data races with philosopher threads.
 * 
 * @param table A pointer to the main enviroment philosopher structure.
 */
void	initialize_semaphores(t_rules *table)
{
	int		i;
	int		forks;
	char	*name;

	forks = table->n_philo;
	table->sem_death = sem_open("/death", O_CREAT | O_EXCL, 0644, 1);
	if (table->sem_death == SEM_FAILED)
		forcend(NULL, PH_SEM_IERR);
	table->sem_print = sem_open("/print", O_CREAT | O_EXCL, 0644, 1);
	if (table->sem_print == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	table->sem_forks = sem_open("/forks", O_CREAT | O_EXCL, 0644, forks / 2);
	if (table->sem_forks == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	table->sem_start = sem_open("/start", O_CREAT | O_EXCL, 0644, 0);
	if (table->sem_start == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	i = -1;
	while (++i < table->n_philo + 1)
	{
		name = table->sem_names[i];
		table->sem_philo[i] = sem_open(name, O_CREAT | O_EXCL, 0644, 1);
		if (table->sem_philo[i] == SEM_FAILED)
			forcend(table, PH_SEM_IERR);
	}
}
