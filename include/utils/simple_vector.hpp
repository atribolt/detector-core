#pragma once

template<class ItemT>
class simple_vector final {
public:
  using value_type = ItemT;

public:
  simple_vector() = default;

  simple_vector(int size, const value_type& value = {})
  {
    resize(size, value);
  }

  simple_vector(simple_vector&&) = delete;
  simple_vector(const simple_vector&) = delete;

  simple_vector& operator=(simple_vector&&) = delete;
  simple_vector& operator=(const simple_vector&) = delete;

  ~simple_vector()
  {
    clear();
  }

  int size() const
  {
    return _size;
  }

  value_type* data()
  {
    return _data;
  }

  const value_type* data() const
  {
    return _data;
  }

  value_type& operator[](int index)
  {
    return _data[index];
  }

  const value_type& operator[](int index) const
  {
    return _data[index];
  }

  void resize(int size, const value_type& value = {})
  {
    if (_size != size) {
      clear();
      _data = new value_type[size] { value };
      _size = size;
    }
  }

  void clear()
  {
    delete[] _data;

    _data = nullptr;
    _size = 0;
  }

private:
  value_type* _data { nullptr };
  int _size { 0 };
};
