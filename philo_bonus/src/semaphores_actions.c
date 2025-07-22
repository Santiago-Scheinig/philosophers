/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphores_actions.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sscheini <sscheini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 18:47:45 by sscheini          #+#    #+#             */
/*   Updated: 2025/07/22 21:04:29 by sscheini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_sem.h"

/**
 * Using the T_SEM_TIME enum, executes instructions on the last_meal timestamp
 * variable, saved on the T_PHILOSOPHER structure, avoiding data races using
 * a semaphore.
 * 
 * @param seat A pointer to a T_PHILOSOPHER structure linked to the last_meal
 * timestamp variable.
 * @param action The action to perfrom into [last_meal_time].
 * @return If is_eating is true, returns the current timeval. Else, returns 
 * the [last_meal_time], on seat, in timeval.
 */
struct timeval	to_time_value(t_philosopher *seat, t_sem_time action)
{
	struct timeval	val;

	gettimeofday(&val, NULL);
	if (safe_sem_wait(seat->sem_philo, "/philo", seat->id))
		exit(PH_SEM_WERR);
	if (action == SEM_TIME_ISEATING)
		seat->is_eating = 1;
	if (action == SEM_TIME_ISFULL)
	{
		seat->is_eating = 0;
		seat->last_meal_time = val;
	}
	if (action == SEM_TIME_IS && !seat->is_eating)
		val = seat->last_meal_time;
	if (safe_sem_post(seat->sem_philo, "/philo", seat->id))
		exit(PH_SEM_WERR);
	return (val);
}

/**
 * Returns the current time of the dinner, using a start_time, in ms.
 * 
 * @param start_time The time in which the dinner started.
 * @return The current time in ms. 
*/
static long	get_time_of_dinner(struct timeval start_time)
{
	struct timeval	tv;
	long			ms_start;
	long			ms_tv;

	gettimeofday(&tv, NULL);
	ms_tv = (tv.tv_sec * 1000L) + (tv.tv_usec / 1000);
	ms_start = (start_time.tv_sec * 1000L) + (start_time.tv_usec / 1000);
	return (ms_tv - ms_start);
}

/**
 * Terminates the program by signaling the monitor with the semaphore /death,
 * then it prints the death message.
 * 
 * @param seat A pointer to the T_PHILOSOPHER linked to the message.
 * @param ms_tv The current time in ms.
 * @note After execution it exits, leaving /print semaphore in 0 and avoiding
 * furhter messages.
 */
static void	print_death_status(t_philosopher *seat, long ms_tv)
{
	if (safe_sem_post(seat->table->sem_death, "/death", -1))
		exit(PH_SEM_PERR);
	printf("%06lims %i died.\n", ms_tv, seat->id);
	exit(PH_SUCCESS);
}

/**
 * Using the T_SEM_PRINT enum, prints a specific message on STDOUT linked to
 * the pointer to a T_PHILOSOPHER seat, avoiding print chunks using 
 * the /print semaphore.
 * 
 * @param seat A pointer to the T_PHILOSOPHER linked to the message.
 * @param action The action that specifies the message to print.
 */
void	to_print_access(t_philosopher *seat, t_sem_print action)
{
	long	ms_tv;

	ms_tv = get_time_of_dinner(seat->start_time);
	if (safe_sem_wait(seat->table->sem_print, "/print", -1))
		exit(PH_SEM_WERR);
	usleep(100);
	if (action == SEM_PRINT_DEATH)
		print_death_status(seat, ms_tv);
	else if (action == SEM_PRINT_EAT)
		printf("%06lims %i is eating.\n", ms_tv, seat->id);
	else if (action == SEM_PRINT_FORK)
	{
		printf("%06lims %i has taken a fork.\n", ms_tv, seat->id);
		if (seat->table->n_philo > 1)
			printf("%06lims %i has taken a fork.\n", ms_tv, seat->id);
	}
	else if (action == SEM_PRINT_SLEEP)
		printf("%06lims %i is sleeping.\n", ms_tv, seat->id);
	else if (action == SEM_PRINT_THINK)
		printf("%06lims %i is thinking.\n", ms_tv, seat->id);
	if (safe_sem_post(seat->table->sem_print, "/print", -1))
		exit(PH_SEM_PERR);
}
