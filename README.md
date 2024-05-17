## Task Scheduler

A class responsible for executing data-related tasks.

## Public Interface TTaskScheduler

- **add** - accepts a task as an argument. Returns an object describing the added task.
- **getFutureResult< T >** - returns an object from which the result of the task, provided as a result of type T, can be obtained in the future.
- **getResult< T >** - returns the result of the task of a certain type. Computes it if it is not yet calculated.
- **executeAll** - executes all scheduled tasks.

## Requirements and Restrictions for Tasks

- [Callable object](https://en.cppreference.com/w/cpp/named_req/Callable)
- Arguments are guaranteed to be [CopyConstructible](https://en.cppreference.com/w/cpp/named_req/CopyConstructible)
- The return value is arbitrary, guaranteed to be [CopyConstructible](https://en.cppreference.com/w/cpp/named_req/CopyConstructible)
- Number of arguments no more than 2

## Restrictions

It is prohibited to use the standard library, except for [containers](https://en.cppreference.com/w/cpp/container) and [smart pointers](https://en.cppreference.com/w/cpp/memory).

## Tests

**Google-tests** are connected to the project.
