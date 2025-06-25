/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 15:59:05 by sscheini          #+#    #+#             */
/*   Updated: 2025/06/25 18:13:52 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	destroy_mutex(t_rules *table)
{
	int	i;
	int ans;

	ans = 0;
	i = -1;
	if (pthread_mutex_destroy(&(table->death_mutex)) != 0)
	{
		ans = 1;
		printf("Error: Unable to destroy %s mutex\n", "death");
	}
	if (pthread_mutex_destroy(&(table->print_mutex)) != 0)
	{
		ans = 1;
		printf("Error: Unable to destroy %s mutex\n", "print");
	}
	while (++i < table->n_forks)
		if (pthread_mutex_destroy(&(table->forks[i])) != 0)
			ans = printf("Error: Unable to destroy fork mutex %d\n", i);
	free(table->forks);
	table->forks = NULL;
	if (ans)
		forcend(table, PH_MUTEXDERR);
}

static void	initialize_mutex(t_rules *table)
{
	int	i;

	table->n_forks = 0;
	if (pthread_mutex_init(&(table->death_mutex), NULL) != 0)
		forcend(NULL, PH_MUTEXIERR);
	if (pthread_mutex_init(&(table->print_mutex), NULL) != 0)
	{
		if (pthread_mutex_destroy(&(table->death_mutex)))
			printf("Error: Unable to destroy mutex\n");
		forcend(table, PH_MUTEXIERR);
	}
	table->forks = malloc(sizeof(pthread_mutex_t) * table->n_philo);
	if (!table->forks)
		forcend(table, PH_MEMFAIL);
	i = -1;
	while (++i < table->n_philo)
	{
		if (pthread_mutex_init(&(table->forks[i]), NULL) != 0)
			forcend(table, PH_MUTEXIERR);
		table->n_forks++;
	}
}

static void	check_args(int argc, char **argv, t_rules *table_rules)
{
	int	i;

	if (argc < 5 || argc > 6)
		forcend(NULL, PH_INVARGC);
	i = 0;
	memset(table_rules, 0, sizeof(t_rules));
	table_rules->death_flag = 0;
	table_rules->n_philo = ft_atoi(argv[++i]);
	if (table_rules->n_philo <= 0)
		forcend(NULL, PH_INVARGV);
	table_rules->time_to_die = ft_atoi(argv[++i]);
	if (table_rules->time_to_die <= 0)
		forcend(NULL, PH_INVARGV);
	table_rules->time_to_eat = ft_atoi(argv[++i]);
	if (table_rules->time_to_eat < 0)
		forcend(NULL, PH_INVARGV);
	table_rules->time_to_sleep = ft_atoi(argv[++i]);
	if (table_rules->time_to_sleep < 0)
		forcend(NULL, PH_INVARGV);
	table_rules->meals_required = ft_atoi(argv[++i]);
	if (table_rules->meals_required < 0)
		forcend(NULL, PH_INVARGV);
}

void	forcend(t_rules *table, int errmsg)
{
	const char	*msg[] = {
		"Success",
		"Invalid amount of arguments",
		"Invalid arguments",
		"Mutex initialization failed",
		"Mutex destruction failed",
		"Memory allocation failed",
	};
	if (table)
		if (table->forks)
			destroy_mutex(table);
	if (errmsg)
		printf("Error: %s\n", msg[errmsg]);
	exit(errmsg);
}

int	main(int argc, char **argv)
{
	t_rules	table_rules;

	check_args(argc, argv, &table_rules);
	initialize_mutex(&table_rules);
	start_philosophical_experiment(&table_rules);
	destroy_mutex(&table_rules);
	return (PH_SUCCESS);
}
