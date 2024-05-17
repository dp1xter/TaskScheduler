#pragma once
#include "any_type.h"

namespace TaskTypes {
    template<typename T>
    concept ExecutableType = requires() {
        typename T::returnable_type;
    };

    template<CopyConstructible FunctionalObject>
    constexpr void ValidateArgumentsALTask() noexcept {
        static_assert(std::is_invocable_v<FunctionalObject>, "The function must be invocable.");
    }

    template<CopyConstructible FunctionalObject, CopyConstructible ArgType>
    constexpr void ValidateArgumentsUTask() noexcept {
        if constexpr (ExecutableType<ArgType>) {
            static_assert(std::is_invocable_v<FunctionalObject, typename ArgType::returnable_type>, "The function must be invocable with a given argument.");
        } else {
            static_assert(std::is_invocable_v<FunctionalObject, ArgType>, "The function must be invocable with a given argument.");
        }
    }

    template<CopyConstructible FunctionalObject, CopyConstructible ArgTypeF, CopyConstructible ArgTypeS>
    constexpr void ValidateArgumentsBTask() noexcept {
        if constexpr (ExecutableType<ArgTypeF> && ExecutableType<ArgTypeS>) {
            static_assert(std::is_invocable_v<FunctionalObject, typename ArgTypeF::returnable_type, typename ArgTypeS::returnable_type>, "The function must be invocable with the given arguments.");
        } else if constexpr (ExecutableType<ArgTypeF>) {
            static_assert(std::is_invocable_v<FunctionalObject, typename ArgTypeF::returnable_type, ArgTypeS>, "The function must be invocable with the given arguments.");
        } else if constexpr (ExecutableType<ArgTypeS>) {
            static_assert(std::is_invocable_v<FunctionalObject, ArgTypeF, typename ArgTypeS::returnable_type>, "The function must be invocable with the given arguments.");
        } else {
            static_assert(std::is_invocable_v<FunctionalObject, ArgTypeF, ArgTypeS>, "The function must be invocable with the given arguments.");
        }
    }

    class AbstractTask {
    public:
        virtual void Execute() = 0;

        [[nodiscard]] virtual AnyType GetResult() {
            Execute();

            return result_;
        }

        [[nodiscard]] virtual bool IsDone() const noexcept {
            return static_cast<bool> (result_);
        }
    protected:
        AnyType result_;
    };

    template<CopyConstructible FunctionalObject>
    class ArgumentLessTask : public AbstractTask {
    public:
        explicit ArgumentLessTask(FunctionalObject functional_object) :
            functional_object_(functional_object)
        {
            ValidateArgumentsALTask<FunctionalObject>();
        }

        ArgumentLessTask(const ArgumentLessTask& other) = default;

        ArgumentLessTask(ArgumentLessTask&& other) noexcept = default;

        void Execute() override {
            if (IsDone()) return;

            result_ = functional_object_();
        }
    private:
        FunctionalObject functional_object_;
    };

    template<CopyConstructible FunctionalObject, CopyConstructible ArgType>
    class UnaryTask : public AbstractTask {
    public:
        explicit UnaryTask(FunctionalObject functional_object, ArgType argument) :
            functional_object_(functional_object),
            argument_(argument)
        {
            ValidateArgumentsUTask<FunctionalObject, ArgType>();
        }

        UnaryTask(const UnaryTask& other) = default;

        UnaryTask(UnaryTask&& other) noexcept = default;

        void Execute() override {
            if (IsDone()) return;

            if constexpr (ExecutableType<ArgType>) {
                result_ = functional_object_(argument_.template GetValue());
            } else {
                result_ = functional_object_(argument_);
            }
        }
    private:
        FunctionalObject functional_object_;
        ArgType argument_;
    };

    template<CopyConstructible FunctionalObject, CopyConstructible ArgTypeF, CopyConstructible ArgTypeS>
    class BinaryTask : public AbstractTask {
    public:
        explicit BinaryTask(FunctionalObject functional_object, ArgTypeF argument_f, ArgTypeS argument_s) :
            functional_object_(functional_object),
            argument_f_(argument_f),
            argument_s_(argument_s)
        {
            ValidateArgumentsBTask<FunctionalObject, ArgTypeF, ArgTypeS>();
        }

        BinaryTask(const BinaryTask& other) = default;

        BinaryTask(BinaryTask&& other) noexcept = default;

        void Execute() override {
            if (IsDone()) return;

            if constexpr (ExecutableType<ArgTypeF> && ExecutableType<ArgTypeS>) {
                result_ = functional_object_(argument_f_.template GetValue(), argument_s_.template GetValue());
            } else if constexpr (ExecutableType<ArgTypeF>) {
                result_ = functional_object_(argument_f_.template GetValue(), argument_s_);
            } else if constexpr (ExecutableType<ArgTypeS>) {
                result_ = functional_object_(argument_f_, argument_s_.template GetValue());
            } else {
                result_ = functional_object_(argument_f_, argument_s_);
            }
        }
    private:
        FunctionalObject functional_object_;
        ArgTypeF argument_f_;
        ArgTypeS argument_s_;
    };
}