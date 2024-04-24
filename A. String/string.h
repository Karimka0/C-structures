#include <iostream>
#include <cstring>

class String {
private:

    size_t size_ = 0;
    size_t capacity_ = 1;
    char* array_ = nullptr;

    explicit String(size_t count) : size_(count), capacity_(count + 1), array_(new char[count + 1]) {}

public:

    String() : size_(0), capacity_(1), array_(new char[1]) {
        array_[0] = '\0';
    }

    String(char c) : String(1, c) {}

    String(const char* str): String(strlen(str)) {
        memcpy(array_, str, size_);
        array_[size_] = '\0';
    }

    String(size_t count, const char c): String(count) {
        memset(array_, c, count);
        array_[count] = '\0';
    }

    String(const String& other): String(other.size_) {
        memcpy(array_, other.array_, size_ + 1);
    }

    String& operator=(String other) {
        swap(other);
        return *this;
    }

    void swap(String& other) {
        std::swap(array_, other.array_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    ~String() {
        delete[] array_;
    }

    char& operator[](size_t index) {
        return array_[index];
    }

    const char& operator[](size_t index) const {
        return array_[index];
    }

    const char& front() const {
        return array_[0];
    }

    char& front() {
        return array_[0];
    }

    char& back() {
        return array_[size_ - 1];
    }

    const char& back() const {
        return array_[size_ - 1];
    }

    void push_back(char c) {
        if (size_ + 1 >= capacity_) {
            capacity_ *= 2;
            char* new_arr = new char[capacity_];
            memcpy(new_arr, array_, size_);
            delete[] array_;
            array_ = new_arr;
        }

        array_[size_] = c;
        array_[++size_] = '\0';
    }

    void pop_back() {
        if (size_ > 0) {
            array_[--size_] = '\0';
        }
    }

    String& operator+=(char c) {
        push_back(c);
        return *this;
    }

    String& operator+=(const String& other) {
        if (size_ + other.size_ >= capacity_) {
            capacity_ = size_ + other.size_ + 1;
            char* newarr = new char[capacity_];
            memcpy(newarr, array_, size_ + 1);
            delete[] array_;
            array_ = newarr;
        }
        memmove(array_ + size_, other.array_, other.size_ + 1);
        size_ += other.size_;
        return *this;
    }

    const char* str_arr() const {
        return array_;
    }

    char* data() {
        return array_;
    }

    const char* data() const {
        return array_;
    }

    void clear() {
        size_ = 0;
        array_[0] = '\0';
    }

    void shrink_to_fit() {
        if (capacity_ > size_ + 1) {
            capacity_ = size_ + 1;
            char* new_arr = new char[capacity_];
            memcpy(new_arr, array_, size_);
            delete[] array_;
            array_ = new_arr;
        }
    }

    size_t find(const String& substr) const {
        const char* found = strstr(array_, substr.array_);
        return (found != nullptr) ? (found - array_) : size_;
    }

    size_t rfind(const String& substr) const {
        if (size_ < substr.size_) {
            return size_;
        }
        for (size_t i = size_ - substr.size_; i > 0; --i) {
            if (strncmp(array_ + i, substr.array_, substr.size_) == 0) {
                return i;
            }
        }
        if (strncmp(array_, substr.array_, substr.size_) == 0) {
            return 0;
        }
        return size_;
    }

    String substr(size_t start, size_t count) const {
        count = std::min(count, size_ - start);
        String sub_arr(count, ' ');
        memcpy(sub_arr.array_, array_ + start, count);
        for (size_t i = start; i < start + count; ++i) {
            sub_arr[i - start] = array_[i];
        }
        return sub_arr;
    }

    bool empty() const {
        return size_ == 0;
    }

    size_t length() const {
        return size_;
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_ - 1;
    }
};

std::ostream& operator<<(std::ostream& os, const String& str) {
    return os << str.str_arr();
}

std::istream& operator>>(std::istream& is, String& str) {
    str.clear();
    int symb = is.get();
    while (std::isspace(symb)) {
        symb = is.get();
    }
    while (symb != EOF && !std::isspace(symb)) {
        str.push_back(static_cast<char>(symb));
        symb = is.get();
    }
    return is;
}

String operator+(const String& left, const String& right) {
    String result = left;
    result += right;
    return result;
}

bool operator==(const String& left, const String& right) {
    return left.length() == right.length() && strcmp(left.data(), right.data()) == 0;
}

bool operator!=(const String& left, const String& right) {
    return !(left == right);
}

bool operator<(const String& left, const String& right) {
    return strcmp(left.data(), right.data()) < 0;
}

bool operator>(const String& left, const String& right) {
    return right < left;
}

bool operator<=(const String& left, const String& right) {
    return !(left > right);
}

bool operator>=(const String& left, const String& right) {
    return !(left < right);
}