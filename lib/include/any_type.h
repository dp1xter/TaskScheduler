#pragma once
#include <memory>

template<typename T>
concept CopyConstructible = std::is_copy_constructible_v<T>;

class AnyType {
public:
    AnyType() noexcept = default;

    template<typename T>
    explicit AnyType(const T& object) {
        ptr_ = std::make_shared<DerivedType<T>>(object);
    }

    AnyType(const AnyType& other) {
        if (other.ptr_) ptr_ = other.ptr_->CloneObject();
        else ptr_ = nullptr;
    }

    AnyType(AnyType&& other) noexcept {
        if (other.ptr_) ptr_ = other.ptr_->CloneObject();
        else ptr_ = nullptr;

        other.ptr_ = nullptr;
    }

    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    template<typename T>
    AnyType& operator=(const T& object) {
        ptr_ = std::make_shared<DerivedType<T>>(object);

        return *this;
    }

    AnyType& operator=(AnyType&& object) noexcept {
        if (ptr_ != object.ptr_) {
            ptr_ = object.ptr_;
            object.ptr_ = nullptr;
        }

        return *this;
    }

    template<typename T>
    auto GetByType() {
        auto derived = std::dynamic_pointer_cast<DerivedType<T>>(ptr_);

        if (!derived) throw std::bad_cast();

        return derived->GetObject();
    }
private:
    class BaseType {
    public:
        [[nodiscard]] virtual std::shared_ptr<BaseType> CloneObject() const = 0;

        virtual ~BaseType() noexcept = default;
    };

    template<CopyConstructible T>
    class DerivedType : public BaseType {
    public:
        explicit DerivedType(const T& object) : object_(object) {}

        [[nodiscard]] std::shared_ptr<BaseType> CloneObject() const override {
            return std::make_shared<DerivedType<T>>(object_);
        }

        [[nodiscard]] T GetObject() const noexcept {
            return object_;
        }
    private:
        T object_;
    };

    std::shared_ptr<BaseType> ptr_ = nullptr;
};