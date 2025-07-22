/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphores_init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 15:23:49 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 19:59:11 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

/**
 * Generates a unique name for a semaphore.
 * 
 * @param base_name The type that identifies the semaphore.
 * @param name A pointer to an allocated string where to save the name.
 * @param id The id that identifies the semaphore.
 * @return The unique name for a semaphore based on parameters.
 */
static char	*get_sem_name(const char *base_name, char *name, int id)
{
	int			name_len;
	int			i;

	i = -1;
	while (base_name[++i])
		name[i] = base_name[i];
	name_len = i;
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
static void	initialize_sem_philo(t_rules *table)
{
	static int	i = -1;
	char		*name;

	table->sem_names = malloc((table->n_philo + 2) * sizeof(char *));
	if (!table->sem_names)
		forcend(table, PH_MEM_AERR);
	table->sem_names[table->n_philo + 1] = NULL;
	while(++i < table->n_philo + 1)
	{
		table->sem_names[i] = malloc(20 * sizeof(char));
		if (!table->sem_names[i])//make a troubleshoot function that also sem_close and unlinks the created sem
		{
			while (--i >= 0)
				free(table->sem_names[i]);
			forcend(table, PH_MEM_AERR);
		}
		memset(table->sem_names[i], 0, (20 * sizeof(char)));
		table->sem_names[i] = get_sem_name("/philo_", table->sem_names[i], i);
		name = table->sem_names[i];
		table->sem_philo[i] = sem_open(name, O_CREAT | O_EXCL, 0644, 1);
		if (table->sem_philo[i] == SEM_FAILED)
			forcend(table, PH_SEM_IERR);
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
		ans += printf("sem: unable to unlink /death semaphore\n");
	if (sem_unlink("/print"))
		ans += printf("sem: unable to unlink /print semaphore\n");
	if (sem_unlink("/forks"))
		ans += printf("sem: unable to unlink /forks semaphore\n");
	if (sem_unlink("/start"))
		ans += printf("sem: unable to unlink /start semaphore\n");
	if (sem_unlink("/ready"))
		ans += printf("sem: unable to unlink /ready semaphore\n");
	i = -1;
	while (++i < table->n_philo + 1)
	{
		name = table->sem_names[i];
		if (sem_unlink(name))
			ans += printf("sem: unable to unlink %s semaphore\n", name);
	}
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
	char	*name;
	int		i;
	int		ans;

	ans = 0;
	if (sem_close(table->sem_death))
		ans += printf("sem: unable to close /death semaphore\n");
	if (sem_close(table->sem_print))
		ans += printf("sem: unable to close /print semaphore\n");
	if (sem_close(table->sem_forks))
		ans += printf("sem: unable to close /forks semaphore\n");
	if (sem_close(table->sem_start))
		ans += printf("sem: unable to close /start semaphore\n");
	if (sem_close(table->sem_ready))
		ans += printf("sem: unable to close /ready semaphore\n");
	i = -1;
	while (++i < table->n_philo + 1)
	{
		name = table->sem_names[i];
		if (sem_close(table->sem_philo[i]))
			ans += printf("sem: unable to close %s semaphore\n", name);
	}
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
	int		forks;

	forks = table->n_philo;
	table->sem_death = sem_open("/death", O_CREAT | O_EXCL, 0644, 0);
	if (table->sem_death == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	table->sem_print = sem_open("/print", O_CREAT | O_EXCL, 0644, 1);
	if (table->sem_print == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	table->sem_forks = sem_open("/forks", O_CREAT | O_EXCL, 0644, forks / 2);
	if (table->sem_forks == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	table->sem_start = sem_open("/start", O_CREAT | O_EXCL, 0644, 0);
	if (table->sem_start == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	table->sem_ready = sem_open("/ready", O_CREAT | O_EXCL, 0644, 0);
	if (table->sem_ready == SEM_FAILED)
		forcend(table, PH_SEM_IERR);
	table->sem_philo = malloc(sizeof(sem_t) * (table->n_philo + 1));
	if (!table->sem_philo)
		forcend(table, PH_MEM_AERR);
	initialize_sem_philo(table);
}
