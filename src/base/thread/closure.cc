#include "base/thread/closure.h"

#include <utility>

#include "base/thread/message_loop.h"
#include "base/thread/message_loop_manager.h"
#include "base/utils.h"

namespace base {
Closure::Closure()
    : is_null_(true),
      task_(nullptr),
      callback_(nullptr),
      timestamp_active_(Now()),
      from_thread_id_(GetTaskCurrentThreadId()) {}

Closure::Closure(const OnceCallback& task, const OnceCallback& callback)
    : from_thread_id_(GetTaskCurrentThreadId()),
      timestamp_active_(Now()),
      task_(std::move(task)),
      callback_(std::move(callback)),
      is_null_(false) {}

Closure::Closure(const TimeDelta& delay,
                 const OnceCallback& task,
                 const OnceCallback& callback)
    : from_thread_id_(GetTaskCurrentThreadId()),
      timestamp_active_(Now() + delay.InMicroseconds()),
      task_(std::move(task)),
      callback_(std::move(callback)),
      is_null_(false) {}

Closure::~Closure() {}

void Closure::RunTask() const {
  if(is_null_)
    return;

  if (task_) {
    task_();
  }

  RunCallback();
}

void Closure::RunCallback() const {
  // Call callback task on from thread
  if (callback_) {
    auto* message_loop =
        MessageLoopManagerSingleton::GetInstance()->GetMessageLoop(
            from_thread_id_);
    if (message_loop) {
      message_loop->PostTask(callback_);
    }
  }
}

}  // namespace base
