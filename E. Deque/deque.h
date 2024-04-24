#include <iostream>
#include <vector>
#include <cstring>

template<typename T>
class Deque {
private:
    static const size_t array_size_ = 16;

    std::vector<T *> data_;
    int left_array_index{};
    int left_index_{};
    int right_array_index{};
    int right_index{};
    size_t deque_size_{};
    size_t external_cap_{};

public:

    Deque() : left_array_index(0), left_index_(0), right_array_index(0), right_index(0), deque_size_(0),
              external_cap_(1) {
        data_.resize(external_cap_);
        try {
            data_[0] = reinterpret_cast<T *>(new char[array_size_ * sizeof(T)]);
        } catch (...) {
            delete data_[0];
            right_array_index = left_array_index - 1;
            throw;
        }
    }

    Deque(int n) : left_array_index(0), left_index_(0), right_array_index(n / array_size_),
                   right_index(n % array_size_), deque_size_(n), external_cap_(n / array_size_ + 1) {
        data_.resize(external_cap_);
        int rem;
        for (int i = 0; i < right_array_index; ++i) {
            try {
                rem = i;
                data_[i] = new T[array_size_];
                for (size_t j = 0; j < array_size_; ++j) {
                    try {
                        rem = i;
                        new(data_[i] + j) T();
                    } catch (...) {
                        for (int k = 0; k <= rem; ++k) {
                            for (int t = 0; t < array_size_; ++t) {
                                data_[k][t].~T();
                            }
                        }
                        throw;
                    }
                }
            } catch (...) {
                right_array_index = left_array_index - 1;
                throw;
            }
        }

        try {
            data_[right_array_index] = new T[array_size_];
            for (int j = 0; j < right_index; ++j) {
                try {
                    new(data_[right_array_index] + j) T();
                } catch (...) {
                    for (int k = 0; k < right_array_index; ++k) {
                        for (int t = 0; t < array_size_; ++t) {
                            data_[k][t].~T();
                        }
                        delete data_[k];
                    }
                    throw;
                }
            }
        } catch (...) {
            right_array_index = left_array_index - 1;
            throw;
        }
    }

    Deque(int n, const T &sample) : left_array_index(0), left_index_(0), right_array_index(n / array_size_),
                                    right_index(n % array_size_), deque_size_(n), external_cap_(n / array_size_ + 1) {
        data_.reserve(external_cap_);
        for (int i = 0; i < right_array_index; ++i) {
            data_[i] = reinterpret_cast<T *>(new char[array_size_ * sizeof(T)]);
            for (size_t j = 0; j < array_size_; ++j) {
                data_[i][j] = sample;
            }
        }
        data_[right_array_index] = reinterpret_cast<T *>(new char[array_size_ * sizeof(T)]);
        for (int j = 0; j < right_index; ++j) {
            data_[right_array_index][j] = sample;
        }
    }

    Deque(const Deque &deque) {
        external_cap_ = deque.external_cap_;
        deque_size_ = deque.deque_size_;
        left_index_ = deque.left_index_;
        left_array_index = deque.left_array_index;
        right_array_index = deque.right_array_index;
        right_index = deque.right_index;
        data_.resize(external_cap_);
        for (int i = deque.left_array_index; i <= deque.right_array_index; ++i) {
            data_[i] = reinterpret_cast<T *>(new char[array_size_ * sizeof(T)]);
            std::memcpy(data_[i], deque.data_[i], array_size_ * sizeof(T));
        }
    }

    ~Deque() {
        if (deque_size_ != 0) {
            for (int i = left_array_index; i < right_array_index; ++i) {
                delete[] data_[i];
            }
        } else {
            for (int i = left_array_index; i < right_array_index; ++i) {
                delete data_[i];
            }
        }
    }

    Deque &operator=(const Deque &deque) {
        if (this != &deque) {
            for (int i = left_array_index; i < right_array_index; ++i) {
                delete[] data_[i];
            }
            if (right_index != 0) {
                delete[] data_[right_array_index];
            }
            external_cap_ = deque.external_cap_;
            deque_size_ = deque.deque_size_;
            left_index_ = deque.left_index_;
            left_array_index = deque.left_array_index;
            right_array_index = deque.right_array_index;
            right_index = deque.right_index;
            data_.resize(external_cap_);
            for (int i = deque.left_array_index; i <= deque.right_array_index; ++i) {
                data_[i] = reinterpret_cast<T *>(new char[array_size_ * sizeof(T)]);
                std::memcpy(data_[i], deque.data_[i], array_size_ * sizeof(T));
            }
        }
        return *this;
    }

    size_t size() const {
        return deque_size_;
    }

    T &operator[](size_t index) {
        size_t array_index = left_array_index + (index + left_index_) / array_size_;
        size_t element_index = (index + left_index_) % array_size_;
        return data_[array_index][element_index];
    }

    const T &operator[](size_t index) const {
        size_t array_index = left_array_index + (index + left_index_) / array_size_;
        size_t element_index = (index + left_index_) % array_size_;
        return data_[array_index][element_index];
    }

    void DequeExpand() {
        std::vector<T *> new_data;
        int temp_left_array_index = right_array_index - left_array_index + 1;
        int temp_right_array_index = temp_left_array_index * 2 - 1;
        int temp_external_cap_ = temp_left_array_index * 3;
        new_data.resize(temp_external_cap_);
        size_t now = left_array_index;
        for (int i = temp_left_array_index; i <= temp_right_array_index; ++i, ++now) {
            new_data[i] = data_[now];
        }
        data_ = new_data;
        left_array_index = temp_left_array_index;
        right_array_index = temp_right_array_index;
        external_cap_ = temp_external_cap_;
    }

    void push_back(const T &element) {
        try {
            if (static_cast<size_t>(right_array_index) == external_cap_ - 1 && right_index == array_size_) {
                DequeExpand();
            }
            if (right_index == array_size_) {
                ++right_array_index;
                data_[right_array_index] = reinterpret_cast<T *>(new char[array_size_ * sizeof(T)]);
                right_index = 0;
            }
            data_[right_array_index][right_index] = element;
            ++right_index;
            ++deque_size_;
        } catch (...) {
            throw;
        }

    }

    void pop_back() {
        try {
            if (right_index == 0) {
                if (deque_size_ != 1) {
                    delete[] data_[right_array_index];
                }
                --right_array_index;
                right_index = array_size_ - 1;
            } else {
                --right_index;
            }
            --deque_size_;
        } catch (...) {
            throw;
        }

    }

    void push_front(const T &element) {
        if (left_array_index == 0 && left_index_ == 0) {
            DequeExpand();
        }
        if (left_index_ == 0) {
            --left_array_index;
            data_[left_array_index] = reinterpret_cast<T *>(new char[array_size_ * sizeof(T)]);
            left_index_ = array_size_;
        }
        --left_index_;
        data_[left_array_index][left_index_] = element;
        ++deque_size_;
    }

    void pop_front() {
        if (left_index_ == array_size_ - 1) {
            delete[] data_[left_array_index];
            ++left_array_index;
            left_index_ = 0;
        } else {
            ++left_index_;
        }
        --deque_size_;
    }

    T &at(size_t index) {
        if (index >= size() || index < 0) {
            throw std::out_of_range("Index out of range");
        } else {
            size_t array_index = left_array_index + (index + left_index_) / array_size_;
            size_t element_index = (index + left_index_) % array_size_;
            return data_[array_index][element_index];
        }
    }

    const T &at(size_t index) const {
        if (index >= size() || index < 0) {
            throw std::out_of_range("Index out of range");
        } else {
            size_t array_index = left_array_index + (index + left_index_) / array_size_;
            size_t element_index = (index + left_index_) % array_size_;
            return data_[array_index][element_index];
        }
    }

    template<bool is_const>
    class base_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename std::conditional<is_const, const T, T>::type;
        using difference_type = int;
        using pointer = typename std::conditional<is_const, const T *, T *>::type;
        using reference = typename std::conditional<is_const, const T &, T &>::type;


        base_iterator() : ptr(nullptr) {}

        base_iterator(T *const *ptr, int index) : ptr(ptr), index(index) {}

        base_iterator(const base_iterator<false> &other) {
            ptr = other.ptr;
            index = other.index;
        }

        base_iterator &operator=(const base_iterator<false> &other) {
            ptr = other.ptr;
            index = other.index;
            return *this;
        }

        ~base_iterator() = default;

        base_iterator &operator++() {
            if (index == array_size_ - 1) {
                ++ptr;
                index = 0;
            } else {
                ++index;
            }
            return *this;
        }

        base_iterator operator++(int) {
            base_iterator temp = *this;
            ++*this;
            return temp;
        }

        base_iterator &operator--() {
            if (index == 0) {
                index = array_size_ - 1;
                --ptr;
            } else {
                --index;
            }
            return *this;
        }

        base_iterator operator--(int) {
            base_iterator temp = *this;
            --(*this);
            return temp;
        }

        reference operator*() const {
            return (*ptr)[index];
        }

        pointer operator->() {
            if constexpr (std::is_const_v<value_type>) {
                return &(*ptr)[index];
            } else {
                return (*ptr) + index;
            }
        }

        bool operator==(const base_iterator &other) const {
            return (ptr == other.ptr && index == other.index);
        }

        bool operator!=(const base_iterator &other) const {
            return !(*this == other);
        }

        bool operator<(const base_iterator &other) const {
            return (ptr < other.ptr || (ptr == other.ptr && index < other.index));
        }

        bool operator<=(const base_iterator &other) const {
            return (*this == other || *this < other);
        }

        bool operator>(const base_iterator &other) const {
            return !(*this <= other);
        }

        bool operator>=(const base_iterator &other) const {
            return !(*this < other);
        }

        base_iterator operator+(int n) const {
            base_iterator temp = *this;
            temp += n;
            return temp;
        }

        base_iterator operator-(int n) const {
            base_iterator temp = *this;
            temp -= n;
            return temp;
        }

        int operator-(base_iterator other) const {
            int diff1 = ptr - other.ptr;
            int diff2 = index - other.index;
            return diff1 * array_size_ + diff2;
        }

        base_iterator &operator-=(int n) {
            if (index >= n) {
                index -= n;
            } else {
                int reverse_ind = array_size_ - index - 1;
                ptr -= (reverse_ind + n) / array_size_;
                index = array_size_ - (reverse_ind + n) % array_size_ - 1;
            }
            return *this;
        }

        base_iterator &operator+=(int n) {
            ptr += (index + n) / array_size_;
            index = (index + n) % array_size_;
            return *this;
        }

    private:
        T *const *ptr;
        int index{};
    };

    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        T *const *now_ptr = &data_[left_array_index];
        return iterator(now_ptr, left_index_);
    }

    iterator end() {
        T *const *now_ptr = &data_[right_array_index];
        return iterator(now_ptr, right_index);
    }

    const_iterator begin() const {
        T *const *now_ptr = &data_[left_array_index];
        return const_iterator(now_ptr, left_index_);
    }

    const_iterator end() const {
        T *const *now_ptr = &data_[right_array_index];
        return const_iterator(now_ptr, right_index);
    }

    const_iterator cbegin() {
        T *const *now_ptr = &data_[left_array_index];
        return const_iterator(now_ptr, left_index_);
    }

    const_iterator cend() {
        T *const *now_ptr = &data_[right_array_index];
        return const_iterator(now_ptr, right_index);
    }

    reverse_iterator rbegin() const {
        T *const *now_ptr = &data_[right_array_index];
        return reverse_iterator(iterator(now_ptr, right_index));
    }

    reverse_iterator rend() const {
        T *const *now_ptr = &data_[left_array_index];
        return reverse_iterator(iterator(now_ptr, left_index_));
    }

    reverse_iterator rbegin() {
        T *const *now_ptr = &data_[right_array_index];
        return reverse_iterator(iterator(now_ptr, right_index));
    }

    reverse_iterator rend() {
        T *const *now_ptr = &data_[left_array_index];
        return reverse_iterator(iterator(now_ptr, left_index_));
    }

    const_reverse_iterator crbegin() {
        T *const *now_ptr = &data_[right_array_index];
        return const_reverse_iterator(const_iterator(now_ptr, right_index));
    }

    const_reverse_iterator crend() {
        T *const *now_ptr = &data_[left_array_index];
        return const_reverse_iterator(const_iterator(now_ptr, left_index_));
    }

    void insert(iterator iter, const T &element) {
        iterator iter1 = --end();
        iterator iter2 = end();
        push_back(element);
        while (iter2 != iter) {
            T temp = *iter1;
            *iter1 = *iter2;
            *iter2 = temp;
            --iter1;
            --iter2;
        }
    }

    void erase(iterator iter) {
        iterator iter1 = iter;
        iterator iter2 = ++iter;
        while (iter1 != end()) {
            T temp = *iter1;
            *iter1 = *iter2;
            *iter2 = temp;
            ++iter1;
            ++iter2;
        }
        pop_back();
    }
};