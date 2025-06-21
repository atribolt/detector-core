#pragma once

#include <iterator>

template<class T>
class ring_buffer_iterator {
public:
  using value_type = T;

  ring_buffer_iterator()
    : ring_buffer_iterator(nullptr, 0, 0)
  {}

  ring_buffer_iterator(value_type* data, int size, int index)
    : _items(data)
    , _size(size)
    , _cursor(0)
  {
    move_cursor(index);
  }

  value_type& operator*()
  {
    return _items[_cursor];
  }

  const value_type& operator*() const
  {
    return _items[_cursor];
  }

  ring_buffer_iterator& operator++()
  {
    move_cursor(1);
    return *this;
  }

  ring_buffer_iterator operator++(int) const
  {
    return { _items, _size, _cursor + 1 };
  }

  ring_buffer_iterator& operator--()
  {
    move_cursor(-1);
    return *this;
  }

  ring_buffer_iterator operator--(int) const
  {
    return { _items, _size, _cursor - 1 };
  }

  bool operator==(const ring_buffer_iterator& other) const
  {
    return _items == other._items && _size == other._size && _cursor == other._cursor;
  }

  bool operator!=(const ring_buffer_iterator& other) const
  {
    return !(*this == other);
  }

  value_type* operator->()
  {
    return _items + _cursor;
  }

  const value_type* operator->() const
  {
    return _items + _cursor;
  }

  const value_type* raw()
  {
    return _items + _cursor;
  }

private:
  inline void move_cursor(int offset)
  {
    _cursor = (_cursor + offset) % _size;
  }

private:
  value_type* _items;
  int _size;
  int _cursor;
};

namespace std
{
  template<class T>
  struct iterator_traits<ring_buffer_iterator<T>> {
    using value_type = typename ring_buffer_iterator<T>::value_type;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = int;

    using iterator_concept = std::contiguous_iterator_tag;
    using iterator_category = std::bidirectional_iterator_tag;
  };
}
