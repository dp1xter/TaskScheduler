#pragma once
#include "task_types.h"
#include <type_traits>
#include <vector>

namespace TaskScheduler {
    using namespace TaskTypes;

    class TTaskScheduler {
    public:
        using task_pointer = std::shared_ptr<AbstractTask>;

        TTaskScheduler() = default;

        TTaskScheduler(const TTaskScheduler& other) = default;

        TTaskScheduler(TTaskScheduler&& other) noexcept = default;

        template<CopyConstructible FunctionalObject, typename Validation = std::enable_if_t<std::is_invocable_v<FunctionalObject>>>
        auto add(FunctionalObject functional_object) {
            task_pointers_.push_back(std::make_shared<ArgumentLessTask<FunctionalObject>>(ArgumentLessTask(functional_object)));

            return task_pointers_.back();
        }

        template<CopyConstructible FunctionalObject, CopyConstructible ArgType>
        auto add(FunctionalObject functional_object, ArgType argument) {
            ValidateArgumentsUTask<FunctionalObject, ArgType>();

            task_pointers_.push_back(std::make_shared<UnaryTask<FunctionalObject, ArgType>>(UnaryTask(functional_object, argument)));

            return task_pointers_.back();
        }

        template<CopyConstructible FunctionalObject, CopyConstructible ArgTypeF, CopyConstructible ArgTypeS>
        auto add(FunctionalObject functional_object, ArgTypeF argument_f, ArgTypeS argument_s) {
            ValidateArgumentsBTask<FunctionalObject, ArgTypeF, ArgTypeS>();

            task_pointers_.push_back(std::make_shared<BinaryTask<FunctionalObject, ArgTypeF, ArgTypeS>>(BinaryTask(functional_object, argument_f, argument_s)));

            return task_pointers_.back();
        }

        template<typename T>
        auto getFutureResult(task_pointer& task_ptr) {
            return TaskObjectWrapper<T>(task_ptr);
        }

        template<typename T>
        auto getResult(task_pointer& task_ptr) {
            return task_ptr->GetResult().template GetByType<T>();
        }

        void executeAll() {
            for (auto& task_ptr : task_pointers_) {
                task_ptr->Execute();
            }
        }
    private:
        std::vector<task_pointer> task_pointers_;

        template<typename T>
        class TaskObjectWrapper {
        public:
            using returnable_type = T;

            explicit TaskObjectWrapper(task_pointer& task_ptr) : task_ptr_(task_ptr) {}

            TaskObjectWrapper(const TaskObjectWrapper& other) = default;

            [[nodiscard]] T GetValue() {
                return task_ptr_->GetResult().template GetByType<T>();
            }
        private:
            task_pointer task_ptr_;
        };
    };
}