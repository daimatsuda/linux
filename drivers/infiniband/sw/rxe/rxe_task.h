/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */
/*
 * Copyright (c) 2016 Mellanox Technologies Ltd. All rights reserved.
 * Copyright (c) 2015 System Fabric Works, Inc. All rights reserved.
 */

#ifndef RXE_TASK_H
#define RXE_TASK_H

enum {
	TASK_STATE_START	= 0,
	TASK_STATE_BUSY		= 1,
	TASK_STATE_ARMED	= 2,
};

/*
 * data structure to describe a 'task' which is a short
 * function that returns 0 as long as it needs to be
 * called again.
 */
struct rxe_task {
	struct workqueue_struct	*workq;
	struct work_struct	work;
	int			state;
	spinlock_t		lock;
	void			*arg;
	int			(*func)(void *arg);
	int			ret;
	bool			destroyed;
	/* used to {dis, en}able per-qp work items */
	atomic_t		suspended;
};

int rxe_alloc_wq(void);

void rxe_destroy_wq(void);

/*
 * init rxe_task structure
 *	arg  => parameter to pass to fcn
 *	func => function to call until it returns != 0
 */
int rxe_init_task(struct rxe_task *task, void *arg, int (*func)(void *));

/* cleanup task */
void rxe_cleanup_task(struct rxe_task *task);

/*
 * raw call to func in loop without any checking
 * can call when tasks are suspended
 */
int __rxe_do_task(struct rxe_task *task);

/* run a task without scheduling */
void rxe_run_task(struct rxe_task *task);

/* schedule a task into workqueue */
void rxe_sched_task(struct rxe_task *task);

/* keep a task from scheduling */
void rxe_disable_task(struct rxe_task *task);

/* allow a task to run again */
void rxe_enable_task(struct rxe_task *task);

#endif /* RXE_TASK_H */
