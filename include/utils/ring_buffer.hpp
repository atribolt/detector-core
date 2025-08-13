#pragma once

#include "ring_buffer_iterator.hpp"
#include "simple_vector.hpp"

#include <algorithm>
#include <cstddef>

template<class T, class V>
concept IsContainer = requires(T& t, V v, int id) {
  {
    t.size()
  } -> std::integral;
  {
    t[id]
  } -> std::same_as<V&>;
  t.resize(id);
  t.clear();
};

template<class Item, class ContainerT = simple_vector<Item>>
  requires IsContainer<ContainerT, Item>
class ring_buffer {
public:
  using value_type = Item;
  using container_type = ContainerT;
  using iterator = ring_buffer_iterator<value_type>;

public:
  inline ring_buffer(int max_size = 0)
  {
    init(max_size + 1);
  }

  inline ~ring_buffer() noexcept
  {}

  void clear()
  {
    init(_items.size());
  }

  void reset(size_t max_size)
  {
    init(max_size);
  }

  template<class... Args>
  inline void emplace_back(Args&&... args)
  {
    _items[_cursor] = value_type { args... };
    inc_cursor();
  }

  inline value_type pop_front()
  {
    value_type result;

    std::swap(_items[_front], result);
    inc_front();

    return result;
  }

  inline void push_back(value_type&& elem)
  {
    std::swap(_items[_cursor], elem);
    inc_cursor();
  }

  inline void push_back(const value_type& elem)
  {
    _items[_cursor] = elem;
    inc_cursor();
  }

  inline iterator begin()
  {
    return { _items.data(), _items.size(), _front };
  }

  inline iterator end()
  {
    return { _items.data(), _items.size(), _cursor };
  }

  inline int size() const noexcept
  {
    auto size = max_size();
    return (size + _cursor - _front) % size;
  }

  inline int max_size() const
  {
    return _items.size();
  }

  inline bool empty() const noexcept
  {
    return size() == 0;
  }

  inline value_type operator[](size_t i) const
  {
    return (_front + i) % _items.size();
  }

private:
  inline void init(int max_size)
  {
    _items.resize(max_size);
    _cursor = 0;
    _front = 0;
  }

  inline void inc_cursor() noexcept
  {
    _cursor = (_cursor + 1) % _items.size();

    if (_cursor == _front) {
      _front = (_front + 1) % _items.size();
    }
  }

  inline void inc_front() noexcept
  {
    _front = (_front + 1) % _items.size();
  }

private:
  container_type _items;

  int _cursor = 0;
  int _front = 0;
};
