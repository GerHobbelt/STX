
#pragma once

#include <utility>

#include "stx/config.h"
#include "stx/scheduler.h"

STX_BEGIN_NAMESPACE

using stx::Promise;
using stx::RequestProxy;
using stx::ServiceToken;

namespace sched {

template <typename Fn>
auto fn(TaskScheduler &scheduler, Fn fn_task, TaskPriority priority,
        TaskTraceInfo trace_info) {
  auto timepoint = std::chrono::steady_clock::now();
  TaskId task_id{scheduler.next_task_id};
  scheduler.next_task_id++;
  static_assert(std::is_invocable_v<Fn &>);

  using output = std::invoke_result_t<Fn &>;

  Promise promise{make_promise<output>(scheduler.allocator).unwrap()};
  Future future{promise.get_future()};
  PromiseAny scheduler_promise{promise.share()};

  RcFn<void()> sched_fn =
      fn::rc::make_functor(scheduler.allocator, [fn_task_ = std::move(fn_task),
                                                 promise_ =
                                                     std::move(promise)]() {
        if constexpr (std::is_void_v<std::invoke_result_t<Fn &>>) {
          fn_task_();
          promise_.notify_completed();
        } else {
          promise_.notify_completed(fn_task_());
        }
      }).unwrap();

  scheduler.entries =
      vec::push(std::move(scheduler.entries),
                Task{std::move(sched_fn), fn::rc::make_static(task_is_ready),
                     timepoint, std::move(scheduler_promise), task_id, priority,
                     std::move(trace_info)})
          .unwrap();

  return future;
}

// returns future of invoke result
template <typename Fn, typename... OtherFns>
auto chain(TaskScheduler &scheduler, Chain<Fn, OtherFns...> chain,
           TaskPriority priority, TaskTraceInfo trace_info) {
  auto timepoint = std::chrono::steady_clock::now();
  TaskId task_id{scheduler.next_task_id};
  scheduler.next_task_id++;

  using result_type = typename Chain<Fn, OtherFns...>::last_phase_result_type;
  using stack_type = typename Chain<Fn, OtherFns...>::stack_type;
  static constexpr auto num_phases = Chain<Fn, OtherFns...>::num_phases;

  Promise promise = make_promise<result_type>(scheduler.allocator).unwrap();
  Future future{promise.get_future()};
  PromiseAny scheduler_promise{promise.share()};

  RcFn<void()> fn =
      fn::rc::make_functor(scheduler.allocator, [state_ = ChainState{},
                                                 stack_ = stack_type{Void{}},
                                                 chain_ = std::move(chain),
                                                 promise_ = std::move(
                                                     promise)]() mutable {
        RequestProxy proxy{promise_};

        chain_.resume(stack_, state_, proxy);

        // suspended or canceled
        if (state_.next_phase_index < num_phases) {
          ServiceToken service_token = state_.service_token;

          if (service_token.type == RequestType::Cancel) {
            promise_.notify_canceled();
          } else {
            promise_.notify_suspended();
          }
        } else {
          // completed
          result_type result = std::move(std::get<result_type>(stack_));
          promise_.notify_completed(std::forward<result_type>(result));
        }
      }).unwrap();

  scheduler.entries =
      vec::push(std::move(scheduler.entries),
                Task{std::move(fn), fn::rc::make_static(task_is_ready),
                     timepoint, std::move(scheduler_promise), task_id, priority,
                     std::move(trace_info)})
          .unwrap();

  return future;
}

}  // namespace sched

STX_END_NAMESPACE
