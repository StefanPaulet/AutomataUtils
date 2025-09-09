//
// Created by stefan on 8/27/25.
//

#pragma once
#include <optional>
#include <type_traits>

namespace au {
  class NfaState;

namespace detail {

template <typename T> struct NextStateWrapperContainer {
  NextStateWrapperContainer() = default;
  NextStateWrapperContainer(NextStateWrapperContainer const&) = default;
  NextStateWrapperContainer(NextStateWrapperContainer&&) = default;

  explicit(false) NextStateWrapperContainer(T const& base) : _base{base} {}
  [[nodiscard]] auto& get() const { return _base.get(); }

  std::reference_wrapper<T> _base;
};

template <typename T> requires std::is_trivially_copy_constructible_v<T> struct NextStateWrapperContainer<T> {
  NextStateWrapperContainer() = default;
  NextStateWrapperContainer(NextStateWrapperContainer const&) = default;
  NextStateWrapperContainer(NextStateWrapperContainer&&) = default;

  explicit(false) NextStateWrapperContainer(T base) : _base{base} {}
  [[nodiscard]] auto get() const { return _base; }

  T _base;
};


template <typename T> struct NextStateIterableWrapper {
  struct Iterator {
    bool isValid;
    NextStateWrapperContainer<T> const* _container;

    auto operator*() const { return _container->get(); }
    auto& operator++() { isValid = false; return *this; }
    auto operator!=(Iterator const& other) const {
      return isValid != other.isValid;
    }
  };

  [[nodiscard]] auto begin(std::optional<NextStateWrapperContainer<T> const*> container) const {
    return container.transform([](NextStateWrapperContainer<T> const* container) {
      return Iterator{true, container};
    }).value_or(Iterator{false, nullptr});
  }

  [[nodiscard]] auto end(std::optional<NextStateWrapperContainer<T> const*> container) const {
    return container.transform([](NextStateWrapperContainer<T> const* container) {
      return Iterator{false, container};
    }).value_or(Iterator{false, nullptr});
  }
};

template <typename T>
requires requires(T obj) {
    { obj.begin() };
    { obj.end() };
    typename T::const_iterator;
} struct NextStateIterableWrapper<T> {
  struct Iterator {
    std::optional<typename T::const_iterator> it;
    [[nodiscard]] auto operator*() {
      return *(it.value());
    }
    [[nodiscard]] auto& operator++() { return ++it.value(); }
    [[nodiscard]] auto operator!=(Iterator const& other) { return it != other.it; }
  };

  [[nodiscard]] auto begin(std::optional<NextStateWrapperContainer<T> const*> container) const {
    return Iterator{container.transform([](NextStateWrapperContainer<T> const* const& obj) { return obj->get().begin(); })};
  }

  [[nodiscard]] auto end(std::optional<NextStateWrapperContainer<T> const*> container) const {
    return Iterator{container.transform([](NextStateWrapperContainer<T> const* obj) { return obj->get().end(); })};
  }
};

template <typename T> struct NextStateConvertibleWrapper {
  [[nodiscard]] auto convert(NextStateWrapperContainer<T>& t) {
    return t.get();
  }
  [[nodiscard]] auto convert(NextStateWrapperContainer<T> const& t) const {
    return t.get();
  }
};

template <typename T> requires std::is_pointer_v<T>
struct NextStateConvertibleWrapper<T> {
  [[nodiscard]] auto convert(NextStateWrapperContainer<T>& t) {
    return t.get();
  }
  [[nodiscard]] auto convert(NextStateWrapperContainer<T> const& t) const {
    return static_cast<std::add_pointer_t<std::add_const_t<std::remove_pointer_t<T>>> const>(t.get());
  }
};

} // namespace detail

template <typename BaseStateContainer> class NextStateWrapper :
    private detail::NextStateIterableWrapper<BaseStateContainer>,
    private detail::NextStateConvertibleWrapper<BaseStateContainer> {
private:
  using Container = detail::NextStateWrapperContainer<BaseStateContainer>;

  using IterableBase = detail::NextStateIterableWrapper<BaseStateContainer>;
  using ConvertibleBase = detail::NextStateConvertibleWrapper<BaseStateContainer>;

public:
  NextStateWrapper() : _container{std::nullopt} {}
  explicit(false) NextStateWrapper(Container const& container) : _container{container} {}
  NextStateWrapper(NextStateWrapper const&) = default;
  NextStateWrapper(NextStateWrapper&&) noexcept = default;

  template <typename T>
  requires std::is_convertible_v<BaseStateContainer, T>
  [[nodiscard]] operator T() {
    return ConvertibleBase::convert(_container.value());
  }

  template <typename T>
  requires std::is_convertible_v<BaseStateContainer const, T>
  [[nodiscard]] operator const Container() const {
    return ConvertibleBase::convert(_container.value());
  }

  [[nodiscard]] auto isDead() const { return !_container.has_value(); }
  [[nodiscard]] auto begin() const {
    if (!_container.has_value()) {
      return IterableBase::begin(std::nullopt);
    }
    return IterableBase::begin(&_container.value());
  }
  [[nodiscard]] auto end() const {
    if (!_container.has_value()) {
      return IterableBase::end(std::nullopt);
    }
    return IterableBase::end(&_container.value());
  }
  [[nodiscard]] auto operator->() { return &_container->get(); }
  [[nodiscard]] auto operator->() const { return &_container->get(); }

private:
  std::optional<Container> _container;
};

} // namespace au