#include <iostream>
#include <vector>

class BigInteger {
    static const size_t kBase = 1000000000;
    bool sign_ = true;
    size_t discharges = 0;
    std::vector<long long> number;

    void shift(int digit) {
        if (*this == 0) {
            return;
        }
        std::vector<long long> new_number;
        for (int i = 0; i < digit; ++i) {
            new_number.push_back(0);
        }
        for (long long i : number) {
            new_number.push_back(i);
        }
        number = new_number;
        discharges = number.size();
    }

public:
    BigInteger() =default;

    explicit BigInteger(size_t num) : sign_(true), discharges(1) {
        number.push_back(num % kBase);
        if (num >= kBase) {
            number.push_back(num/kBase);
            ++discharges;
        }
    }

    explicit BigInteger(int num) : sign_(num >= 0), discharges(1) {
        if (!sign_) {
            num = -num;
        }
        if (static_cast<size_t>(num) >= kBase) {
            number.push_back(static_cast<size_t>(num) % kBase);
            num /= kBase;
            discharges++;
        }
        number.push_back(num);
    }

    BigInteger(long long num) : sign_(num >= 0), discharges(1) {
        if (!sign_) {
            num = -num;
        }
        number.push_back(num);
    }

    BigInteger(bool sign_now, size_t size, std::vector<long long> digits_now) : sign_(sign_now), discharges(size){
        for (size_t i = 0; i < digits_now.size(); ++i) {
            number.push_back(digits_now[i]);
        }
    }

    BigInteger& operator+=(const BigInteger& other) {
        BigInteger result;

        if (sign_ && other.sign_) {
            if (*this >= other) {

                result.discharges = other.discharges;
                for(size_t i = 0; i < result.discharges; ++i) {
                    long long sum = number[i] + other.number[i];
                    if (i == 0) {
                        result.number.push_back(sum % kBase);
                        result.number.push_back(sum / kBase);
                    } else {
                        result.number[i] += sum % kBase;
                        result.number.push_back(sum / kBase);
                    }
                }
                ++result.discharges;

                if (discharges != other.discharges) {
                    result.number[result.discharges - 1] += number[result.discharges - 1];
                }

                for (size_t i = result.discharges; i < discharges; ++i) {
                    result.number.push_back(number[i]);
                    ++result.discharges;
                }

                if (result.number.back() == 0) {
                    result.number.pop_back();
                    --result.discharges;
                }
            } else {
                result =  other;
                result += *this;
            }
        } else if (sign_ && !other.sign_) {
            result = *this;
            result -= (-other);
        } else if (! sign_ && other.sign_) {
            result = other;
            result -= (-*this);
        } else {
            result = *this;
            result += other;
            result = -result;
        }
        *this = result;
        return *this;
    }

    BigInteger& operator-=(const BigInteger& other) {
        BigInteger result;
        if (sign_ && other.sign_) {
            if (*this >= other) {
                result.discharges = other.discharges;
                long long shift = 0;

                for (size_t i = 0; i < other.discharges; ++i) {
                    long long diff = number[i] - other.number[i];
                    result.number.push_back(diff + shift);
                    shift = 0;
                    if (result.number[i] < 0) {
                        result.number[i] += kBase;
                        shift = -1;
                    }
                }

                for (size_t i = other.discharges; i < discharges; ++i) {
                    result.number.push_back(number[i] + shift);
                    ++result.discharges;
                    shift = 0;
                    if (result.number[i] < 0) {
                        result.number[i] += kBase;
                        shift = - 1;
                    }
                }

                while (result.number.size() > 1 && result.number.back() == 0) {
                    --result.discharges;
                    result.number.pop_back();
                }

            } else {
                result = other;
                result -= *this;
                result = -result;
            }
        } else if (sign_ && !other.sign_) {
            result = *this;
            result += (-other);
        } else if (!sign_ && other.sign_) {
            result = other;
            result += -*this;
            result = -result;
        } else {
            result = (-other);
            result += *this;
        }

        *this = result;
        return *this;
    }

    BigInteger& operator*=(const BigInteger& other) {
        BigInteger result;
        result.discharges = discharges + other.discharges;
        result.number.resize(result.discharges, 0);

        for (size_t i = 0; i < discharges; ++i) {
            long long shift = 0;
            for (size_t j = 0; j < other.discharges || shift; ++j) {
                long long mult = result.number[i + j] + number[i] * (j < other.discharges ? other.number[j] : 0) + shift;
                result.number[i + j] = mult % kBase;
                shift = mult / kBase;
            }
        }

        while (result.number.size() > 1 && result.number.back() == 0) {
            result.number.pop_back();
        }

        result.discharges = result.number.size();
        result.sign_ = sign_ == other.sign_;
        *this =  result;
        return *this;
    }

    BigInteger& operator/=(const BigInteger& other) {
        if (*this == 0) {
            return *this;
        }

        BigInteger numerator = *this;
        BigInteger denominator = other;
        BigInteger result = 0;
        BigInteger diff = 0;
        BigInteger temp2 = 0;
        if (!numerator.sign_){
            numerator = -numerator;
        }
        if (!denominator.sign_) {
            denominator = -denominator;
        }
        if (numerator < denominator) {
            *this = 0;
            return *this;
        }

        for (size_t i = numerator.discharges; i > 0; --i) {
            diff.shift(1);
            result.shift(1);
            diff += number[i - 1];
            if (diff < denominator){
                continue;
            }
            int left = 0;
            int right = kBase + 1;
            while (right > left) {
                int middle = (left + right) >> 1;
                temp2 = denominator;
                temp2 *= middle;
                if (diff > temp2) {
                    ++middle;
                    left = middle;
                } else {
                    right = middle;
                }
            }
            BigInteger temp = denominator;
            temp *= left;
            if (diff < temp) {
                --left;
                temp -= denominator;
            }
            result += left;
            diff -= temp;
        }
        while (result.number.size() > 1 && result.number.back() == 0) {
            result.number.pop_back();
        }
        result.discharges = result.number.size();
        result.sign_ = this->sign_ == other.sign_;
        *this = result;
        return *this;
    }

    BigInteger& operator%=(const BigInteger& other) {
        BigInteger temp = *this;
        temp /= other;
        temp *= other;
        *this -= temp;
        return *this;
    }

    BigInteger operator-() const {
        BigInteger result = *this;
        if (*this != 0) {
            result.sign_ = !result.sign_;
        }
        return result;
    }

    BigInteger& operator++() {
        *this += 1;
        return *this;
    }

    BigInteger operator++(int) {
        BigInteger temp = *this;
        ++(*this);
        return temp;
    }

    BigInteger& operator--() {
        *this -= 1;
        return *this;
    }

    BigInteger operator--(int) {
        BigInteger temp = *this;
        --(*this);
        return temp;
    }

    bool operator ==(const BigInteger& other) const {
        return sign_ == other.sign_ && number == other.number;
    }

    bool operator!=(const BigInteger& other) const {
        return !(*this == other);
    }

    bool operator<(const BigInteger& other) const {
        if (sign_ != other.sign_) {
            return !sign_ && other.sign_;
        }

        if (discharges != other.discharges) {
            return (discharges < other.discharges) == sign_;
        }

        for (size_t i = discharges; i > 0; --i) {
            if (number[i - 1] != other.number[i - 1]) {
                return (number[i - 1] < other.number[i - 1]) == sign_;
            }
        }
        return false;
    }

    bool operator<=(const BigInteger& other) const {
        return !(other < *this);
    }

    bool operator>(const BigInteger& other) const {
        return other < *this;
    }

    bool operator>=(const BigInteger& other) const {
        return !(*this < other);
    }

    explicit operator bool() const {
        return *this != 0;
    }

    std::string toString() const {
        if (number.empty() || *this == 0) {
            return "0";
        }
        std::string now_str = "";
        if (!sign_ && *this != 0) {
            now_str = "-";
        }

        for (size_t i = discharges; i > 0; --i) {
            if (i <= number.size()) {
                if (i != discharges) {
                    now_str += std::string(9 - len(number[i - 1]), '0');
                }
                now_str += std::to_string(number[i - 1]);
            }
        }
        return now_str;
    }

private:
    static int len(long long num) {
        if (num == 0) {
            return 1;
        }
        int ans = 0;
        while (num > 0) {
            ++ans;
            num /= 10;
        }
        return ans;
    }
};

BigInteger operator+(const BigInteger& left, const BigInteger& right)  {
    BigInteger result = left;
    result += right;
    return result;
}

BigInteger operator-(const BigInteger& left, const BigInteger& right) {
    BigInteger result = left;
    result -= right;
    return result;
}

BigInteger operator*(const BigInteger& left, const BigInteger& right) {
    BigInteger result = left;
    result *= right;
    return result;
}

BigInteger operator/(const BigInteger& left, const BigInteger& right) {
    BigInteger result = left;
    result /= right;
    return result;
}

BigInteger operator%(const BigInteger& left, const BigInteger& right) {
    BigInteger result = left;
    result %= right;
    return result;
}

BigInteger operator""_bi(unsigned long long value) {
    return BigInteger(static_cast<size_t>(value));
}

std::istream& operator>>(std::istream& is, BigInteger& bigInt) {
    std::string input;
    is >> input;

    bool sign = true;
    size_t start = 0;
    if (input.size() != 0 && input[0] == '-') {
        sign = false;
        start = 1;
    }

    std::vector<long long> now_number;
    size_t full = (input.size() - start) / 9;
    size_t rem = (input.size() - start) % 9;

    std::string now_digit = "";
    for (size_t i = full; i != 0; --i) {
        for (int j = 0; j < 9; ++j) {
            if (start + rem + (i - 1) * 9 + j >= 0 && start + rem + (i - 1) * 9 + j < input.size()) {
                now_digit += input[start + rem + (i - 1) * 9 + j];
            }
        }
        now_number.push_back(stoll(now_digit));
        now_digit = "";
    }

    if (rem != 0) {
        for (size_t i = start; i < rem + start; ++i) {
            if (i < input.size()) {
                now_digit += input[i];
            }
        }
        now_number.push_back(stoll(now_digit));
    }

    bigInt = BigInteger(sign, now_number.size(), now_number);
    return is;
}

std::ostream& operator<<(std::ostream& os, const BigInteger& bigInt) {
    os << bigInt.toString();
    return os;
}

class Rational {
private:
    bool sign = true;
    BigInteger numerator = 0;
    BigInteger denominator = 1;

    BigInteger gcd(const BigInteger& a, const BigInteger& b) const {
        if (b == 0) {
            return a;
        }
        return gcd(b, a % b);
    }

    void reduce() {
        BigInteger common = gcd(numerator, denominator);
        numerator /= common;
        denominator /= common;
    }

public:

    Rational() {}

    Rational(int num) : sign(num >= 0), numerator(std::abs(num)), denominator(1) {}

    Rational(const BigInteger& num) : sign(num >= 0), numerator(num), denominator(1) {
        if (num < 0) {
            numerator = -numerator;
        }
    }

    bool operator==(const Rational& other) const {
        if (numerator == 0 && other.numerator == 0) return true;
        return sign == other.sign && numerator * other.denominator == other.numerator * denominator;
    }

    bool operator==(const int& other) const {
        return *this == Rational(other);
    }

    bool operator!=(const Rational& other) const {
        return !(*this == other);
    }

    bool operator!=(const int& other) const {
        return *this != Rational(other);
    }

    bool operator<(const Rational& other) const {
        if (sign != other.sign) {
            return !sign && other.sign;
        }

        return sign ? (numerator * other.denominator < other.numerator * denominator)
                    : (numerator * other.denominator > other.numerator * denominator);
    }

    bool operator>(const Rational& other) const {
        return other < *this;
    }

    bool operator<=(const Rational& other) const {
        return !(other < *this);
    }

    bool operator>=(const Rational& other) const {
        return !(*this < other);
    }

    bool operator>=(const int& other) const {
        return *this >= Rational(other);
    }

    bool operator<=(const int& other) const {
        return *this <= Rational(other);
    }

    bool operator<(const int& other) const {
        return *this < Rational(other);
    }

    bool operator>(const int& other) const {
        return *this > Rational(other);
    }

    Rational operator-() const {
        Rational result = *this;
        if (*this != 0) {
            result.sign = !result.sign;
        }
        return result;
    }

    Rational operator/(const BigInteger& other) const {
        return *this / Rational(other);
    }

    Rational operator*(const BigInteger& other) const {
        return *this * Rational(other);
    }
    Rational operator+(const BigInteger& other) const {
        return *this + Rational(other);
    }

    Rational operator-(const BigInteger& other) const {
        return *this - Rational(other);
    }

    Rational& operator+=(const Rational& other) {
        Rational result;
        if (!other.sign &&!sign) {
            *this = -*this;
            *this += -other;
            *this = -result;
            return *this;
        } else if (!sign) {
            result = other;
            result -= -*this;
            *this = result;
            return *this;
        } else if (!other.sign) {
            *this -= -other;
            return *this;
        }

        if (denominator != other.denominator) {
            BigInteger common_denominator = denominator * other.denominator;
            result.numerator = numerator * other.denominator + other.numerator * denominator;
            result.denominator = common_denominator;
        } else {
            result.numerator = numerator + other.numerator;
            result.denominator = denominator;
        }

        result.reduce();
        *this = result;
        return *this;
    }

    Rational& operator-=(const Rational& other) {
        Rational result;
        if (!other.sign &&!sign) {
            result = -*this;
            result -= -other;
            *this = -result;
            return *this;
        } else if (!sign) {
            result = other;
            result += -*this;
            *this = -result;
            return *this;
        } else if (!other.sign) {
            *this += -other;
            return *this;
        }

        if (denominator != other.denominator) {
            result.numerator = numerator * other.denominator - other.numerator * denominator;
            result.denominator = denominator * other.denominator;
        } else {
            result.numerator = numerator - other.numerator;
            result.denominator = denominator;
        }

        if (result.numerator < 0) {
            result.numerator = - result.numerator;
            result.sign = false;
        }

        result.reduce();
        *this = result;
        return *this;
    }

    Rational& operator/=(const Rational& other) {
        Rational result;
        result.sign = sign == other.sign;
        result.numerator = numerator * other.denominator;
        result.denominator = denominator * other.numerator;

        result.reduce();
        *this = result;
        return *this;
    }

    Rational& operator*=(const Rational& other) {
        Rational result;
        result.sign = sign == other.sign;
        result.numerator = numerator * other.numerator;
        result.denominator = denominator * other.denominator;

        result.reduce();
        *this = result;
        return *this;
    }

    std::string toString() const {
        if (denominator == 1) {
            return (sign ? "" : "-") + numerator.toString();
        } else {
            return (sign ? "" : "-") + numerator.toString() + "/" + denominator.toString();
        }
    }

    std::string asDecimal(size_t precision = 0) const {
        Rational abs_val = abs(*this);
        BigInteger whole_part = abs_val.numerator / abs_val.denominator;
        BigInteger remainder = abs_val.numerator % abs_val.denominator;

        std::string decimal = whole_part.toString();

        if (precision > 0) {
            decimal += ".";

            for (size_t i = 0; i < precision; ++i) {
                remainder *= 10;
                decimal += (remainder / abs_val.denominator).toString();
                remainder = remainder % abs_val.denominator;
            }
        }

        return (sign ? "" : "-") + decimal;
    }

    operator double() const {
        return stof(asDecimal(10));
    }

    static Rational abs(const Rational& rational) {
        Rational result = rational;
        result.sign = true;
        return result;
    }
};

Rational operator+(Rational& left, const Rational& right) {
    Rational result = left;
    result += right;
    return result;
}

Rational operator-(Rational& left, const Rational& right) {
    Rational result = left;
    result -= right;
    return result;
}

Rational operator*(Rational& left, const Rational& right) {
    Rational result = left;
    result *= right;
    return result;
}

Rational operator/(Rational& left, const Rational& right) {
    Rational result = left;
    result /= right;
    return result;
}
