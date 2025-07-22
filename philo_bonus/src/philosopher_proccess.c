/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_proccess.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 17:49:01 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 20:49:59 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"


/**
 * Indicates the philosopher thread to start eating:
 * 
 * - Waits on [sem_fork].
 * 
 * - Sets [is_eating] flag to 1.
 * 
 * - Executes an usleep equal to [time_to_eat].
 * 
 * - Increments the [meals_eaten] value by one.
 * 
 * - Posts on [sem_fork].
 * 
 * - Updates [last_meal_time].
 * 
 * @param seat A pointer to the T_PHILOSOPHER thread structure.
 * @note If [n_philo] equals 1, then a specific execution is made, where
 * the thread grabs just one fork and waits until [time_to_die].
 */
static void	eating(t_philosopher *seat)
{
	if (seat->table->n_philo == 1)
	{
		to_print_access(seat, SEM_PRINT_FORK);
		usleep(seat->table->time_to_die * 1100);
	}
	if (safe_sem_wait(seat->table->sem_forks, "/forks", -1))
		exit(PH_SEM_WERR);
	to_print_access(seat, SEM_PRINT_FORK);
	to_time_value(seat, SEM_TIME_ISEATING);
	to_print_access(seat, SEM_PRINT_EAT);
	usleep((seat->table->time_to_eat * 1000));
	if (seat->table->meals_required)
	{
		if (++seat->meals_eaten >= seat->table->meals_required 
		&& !seat->max_meals_achieved)
		{
			if (safe_sem_post(seat->table->sem_ready, "/ready", -1))
				exit(PH_SEM_PERR);
			seat->max_meals_achieved = 1;
		}
	}
	if (safe_sem_post(seat->table->sem_forks, "/forks", -1))
		exit(PH_SEM_PERR);
	to_time_value(seat, SEM_TIME_ISFULL);
}

/**
 * In case of odd [n_philosopher] amount, it verifies if the thread needs
 * to eat or not. To do so, compares the current time agains the [last_meal]
 * time, and verifies it against ([time_to_die] / 2).
 * 
 * @param seat A pointer to the T_PHILOSOPHER thread structure.
 * @return Returns 0 if it's less than ([time_to_die] / 2). Else, returns 1.
 */
static int	is_hungry(t_philosopher *seat)
{
	struct timeval	tv;
	struct timeval	last_meal;
	int				ms_tv;
	int				ms_last_meal;

	gettimeofday(&tv, NULL);
	ms_tv = (tv.tv_sec * 1000L) + (tv.tv_usec / 1000);
	last_meal = to_time_value(seat, SEM_TIME_IS);
	ms_last_meal = (last_meal.tv_sec * 1000L) + (last_meal.tv_usec / 1000);
	if (ms_tv - ms_last_meal <= (seat->table->time_to_die / 2))
		return (0);
	return (1);
}

/**
 * Initializes the forked proccess.
 * 
 * - Sets [last_meal_time] timeval to -1.
 * 
 * - Creates a personal death thread to check for starvation.
 * 
 * - Signals the monitor it's ready to start via post on [sem_ready].
 * 
 * - Waits on [sem_start] for monitor signal to start the dinner.
 * 
 * - Sets [last_meal_time] to current [start_time].
 * 
 * */
static void	philo_init(t_philosopher *seat)
{
	pthread_t	thd_starved_id;

	seat->last_meal_time.tv_sec = -1;
	seat->last_meal_time.tv_usec = -1;
	if (pthread_create(&(thd_starved_id), NULL, philo_death, seat))
		exit(PH_THD_CERR);
	pthread_detach(thd_starved_id);
	if (safe_sem_post(seat->table->sem_ready, "/ready", -1))
		exit(PH_SEM_PERR);
	if (safe_sem_wait(seat->table->sem_start, "/start", -1))
		exit(PH_SEM_WERR);
	gettimeofday(&(seat->start_time), NULL);
	to_time_value(seat, SEM_TIME_ISFULL);
	to_print_access(seat, SEM_PRINT_THINK);
	if ((seat->id % 2) != 0 && seat->table->n_philo != 1)
		usleep((seat->table->time_to_eat / 2) * 1000);
}

/**
 * The philosopher proccess routine.
 * 
 * - Sets the [last_meal_time] to [start_time].
 * 
 * - If odd, waits ([time_to_eat] / 2) to create eating waves.
 * 
 * - A personal thread checks regulary if proccess died by starvation.
 * 
 * - Thinks, eats and sleeps.
 * 
 * @param seat A pointer to a T_PHILOSOPHER structure linked to the last_meal
 * timestamp variable.
 * @note If [n_philosopher] it's an odd amount, uses is_hungry() to set 
 * priorities between waves.
 */
void	philosophize(t_philosopher *seat)
{
	philo_init(seat);
	while (1)
	{
		eating(seat);
		to_print_access(seat, SEM_PRINT_SLEEP);
		usleep((seat->table->time_to_sleep * 1000));
		if ((seat->table->n_philo % 2))
			while (!is_hungry(seat))
				usleep(100);
		to_print_access(seat, SEM_PRINT_THINK);
	}
}
