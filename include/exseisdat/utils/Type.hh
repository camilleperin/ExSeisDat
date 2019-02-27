////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Utilities for passing type information around dynamically.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_TYPE_HH
#define EXSEISDAT_UTILS_TYPE_HH

#include <cstdint>
#include <type_traits>

namespace exseis {
namespace utils {

/// An enumeration of all the arithmetic types used by ExSeisDat.
enum class Type : uint8_t {
    /// Represents `double`
    Double,
    /// Represents `float`
    Float,

    /// Represents `int64_t`
    Int64,
    /// Represents `uint64_t`
    UInt64,

    /// Represents `int32_t`
    Int32,
    /// Represents `uint32_t`
    UInt32,

    /// Represents `int16_t`
    Int16,
    /// Represents `uint16_t`
    UInt16,

    /// Represents `int8_t`
    Int8,
    /// Represents `uint8_t`
    UInt8,

    /// Temporary! Should be removed.
    Index,
    /// Temporary! Should be removed.
    Copy
};


/// @brief Get the \ref Type of a native data type.
template<typename T>
class Type_from_native;

/// @brief Get the native data type from the \ref Type.
template<Type T>
class Native_from_type;


namespace detail {

/// @brief Get the appropriate Type value from the type `T`.
/// @tparam  T The native type to get the Type value for.
/// @returns An appropriate Type value for T
/// @pre T is a built-in integer or floating point type.
template<typename T>
constexpr Type type_from_native_impl()
{
    static_assert(
        std::is_floating_point<T>() || std::is_integral<T>(),
        "Type_from_native is only defined for built-in floating point and integral types!");

    static_assert(
        sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2 || sizeof(T) == 1,
        "Type_from_native is only supported for 64, 32, 16, or 8 bit types!");

    if (std::is_floating_point<T>::value) {
        switch (sizeof(T)) {
            case sizeof(double):
                return Type::Double;
            case sizeof(float):
                return Type::Float;
            default:
                return static_cast<Type>(-1);
        }
    }

    if (std::is_integral<T>::value) {
        bool is_signed = std::is_signed<T>::value;
        switch (sizeof(T)) {
            case sizeof(int64_t):
                return is_signed ? Type::Int64 : Type::UInt64;
            case sizeof(int32_t):
                return is_signed ? Type::Int32 : Type::UInt32;
            case sizeof(int16_t):
                return is_signed ? Type::Int16 : Type::UInt16;
            case sizeof(int8_t):
                return is_signed ? Type::Int8 : Type::UInt8;
        }
    }
}


template<Type TypeId, typename T = void>
struct native_from_type_impl {
    static_assert(
        sizeof(T) == 0, "Native_from_type called for unknown Type Id.");
};

template<>
struct native_from_type_impl<Type::Double> {
    using type = double;
};

template<>
struct native_from_type_impl<Type::Float> {
    using type = float;
};

template<>
struct native_from_type_impl<Type::Int64> {
    using type = int64_t;
};

template<>
struct native_from_type_impl<Type::UInt64> {
    using type = uint64_t;
};

template<>
struct native_from_type_impl<Type::Int32> {
    using type = int32_t;
};

template<>
struct native_from_type_impl<Type::UInt32> {
    using type = uint32_t;
};

template<>
struct native_from_type_impl<Type::Int16> {
    using type = int16_t;
};

template<>
struct native_from_type_impl<Type::UInt16> {
    using type = uint16_t;
};

template<>
struct native_from_type_impl<Type::Int8> {
    using type = int8_t;
};

template<>
struct native_from_type_impl<Type::UInt8> {
    using type = uint8_t;
};

}  // namespace detail


/// @brief Transforms a built-in type to the equivalent Type enum.
/// @tparam T The built-in type to transform.
template<typename T>
class Type_from_native {
  public:
    /// The Type value
    static constexpr Type value = detail::type_from_native_impl<T>();
};

/// @brief Transforms a Type enum to an equivalent built-in type.
/// @tparam T The Type value to transform.
template<Type T>
class Native_from_type {
  public:
    /// The native type
    using type = typename detail::native_from_type_impl<T>::type;
};


}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_TYPE_HH
