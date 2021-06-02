#pragma once

#include "CompareMacros.hpp"
#include "JsonConvertMacros.hpp"

JS_MACRO_ARGUMENT_NO_WARN
#include <QObject>

#define _QJS_PROP_OPTIONAL
#define _QJS_PROP_REQUIRED REQUIRED

#define _QJS_PROP_IMPL(TYPE, NAME, DEFAULT, ...)                                                                                                                         \
  public:                                                                                                                                                                \
    Q_SIGNAL void on_##NAME##_changed(const TYPE &);                                                                                                                     \
    QJS_Prop<ThisType, TYPE> JS_F(NAME){ this, &ThisType::on_##NAME##_changed, TYPE{ DEFAULT } };                                                                        \
    Q_PROPERTY(TYPE NAME MEMBER JS_F(NAME) NOTIFY on_##NAME##_changed __VA_ARGS__)

// ========================================================================================================
//
// QJsonStruct Public APIS
//
// ========================================================================================================

template<class P, typename T>
struct QJS_Prop
{
  private:
    P *parent;
    void (P::*m_ChangedSignal)(const T &);
    T value;
    const T defaultValue;

  public:
    QJS_Prop(P *parent, void (P::*changedSignal)(const T &), const T &def) : parent(parent), m_ChangedSignal(changedSignal), value(def), defaultValue(def){};

    // clang-format off
          T* operator->()       { return &value; }
    const T* operator->() const { return &value; }

//          T& operator()()       { return value; }
//    const T& operator()() const { return value; }

          T& operator*()       { return value; }
    const T& operator*() const { return value; }

    operator       T()         { return value; }
    operator const T()   const { return value; }

    T & operator=(const T& f)           { return set(f); }
    T & operator=(const T&&f)           { return set(std::move(f)); }
    T & operator=(const QJS_Prop<P, T> &f) { return set(f.value); }

    template<class PP> T & operator=(const QJS_Prop<PP, T>& f) { return set(f.value); }
    template<class PP> T & operator=(const T) { value++; return *this; }

    QJS_Prop<P, T> &operator++() { value++; return *this; }
    QJS_Prop<P, T> &operator--() { value--; return *this; }


    template<typename Y> void operator<<(const Y &another) { value << another; }
    template<typename Y> void operator>>(const Y &another) { value >> another; }
    template<typename V> T operator+=(const V &v) { value += v; return value; }
    template<typename V> T operator-=(const V &v) { value -= v; return value; }
    template<typename V> T operator*=(const V &v) { value *= v; return value; }
    template<typename V> T operator/=(const V &v) { value /= v; return value; }
    template<typename V> T operator&=(const V &v) { value &= v; return value; }
    template<typename V> T operator%=(const V &v) { value %= v; return value; }

    QJsonValue toJson() const { return JsonStructHelper::Serialize(value); }
    void loadJson(const QJsonValue &val) { JsonStructHelper::Deserialize(value, val); }
    // clang-format on

    // clang-format off
    bool operator==(const T& val) const { return val == value ; }
    bool operator!=(const T& val) const { return val != value ; }
    template<typename PP> bool operator==(const QJS_Prop<PP, T>& left) const { return   left.value == value ; }
    template<typename PP> bool operator!=(const QJS_Prop<PP, T>& left) const { return !(left.value == value); }
    // clang-format on

    bool isDefault() const
    {
        return value == defaultValue;
    }

  private:
    T &set(const T &v)
    {
        if (value == v)
            return value;
        value = v;
        (parent->*m_ChangedSignal)(v);
        return value;
    }

  public:
    ///
    /// \brief Change the value of target property when this value has changed.
    ///
    template<typename TTarget>
    inline void ReadBind(TTarget *target, const char *target_prop)
    {
        static_assert(std::is_base_of_v<QObject, TTarget>, "Wrong Usage: Target must be a QObject");

        // Firstly, sync target properties.
        ((QObject *) target)->setProperty(target_prop, value);

        QObject::connect(parent, m_ChangedSignal, [=]() {
            if (auto obj = dynamic_cast<QObject *>(target); obj)
                obj->setProperty(target_prop, value);
        });
    }

    ///
    /// \brief Change the value of current property of something happened in target, triggered by target signal
    ///
    template<typename TTarget, typename Func>
    inline void WriteBind(const TTarget *target, const char *target_prop, Func trigger_signal)
    {
        static_assert(std::is_base_of_v<QObject, TTarget>, "Wrong Usage: Target must be a QObject");

        QObject::connect(target, trigger_signal, [=]() {
            if (auto obj = dynamic_cast<const QObject *>(target); obj)
                set(obj->property(target_prop).value<T>());
        });
    }

    template<typename TTarget, typename Func>
    inline void ReadWriteBind(TTarget *target, const char *target_prop, Func trigger_signal)
    {
        ReadBind(target, target_prop);
        WriteBind(target, target_prop, trigger_signal);
    }
};

#define QJS_PROP(TYPE, NAME, ...) _QJS_PROP_IMPL(TYPE, NAME, TYPE{}, __VA_ARGS__)
#define QJS_PROP_D(TYPE, NAME, DEFAULT, ...) _QJS_PROP_IMPL(TYPE, NAME, DEFAULT, __VA_ARGS__)

#define QJS_FUNCTION_DEFAULT_CONSTRUCTOR(CLASS, ...)                                                                                                 \
    CLASS(){};                                                                                                                                       \
    virtual ~CLASS(){};

#define __QJS_CTOR_B(base) base()
#define __QJS_CTOR_F(name)

#define QJS_FUNCTION(CLASS, ...)                                                                                                                                         \
  private:                                                                                                                                                               \
    using ThisType = CLASS;                                                                                                                                              \
                                                                                                                                                                         \
  public:                                                                                                                                                                \
    QJS_FUNCTION_DEFAULT_CONSTRUCTOR(CLASS, __VA_ARGS__)                                                                                                                 \
    QJS_FUNC_COPY(CLASS, __VA_ARGS__);                                                                                                                                   \
    QJS_FUNC_JSON(CLASS, __VA_ARGS__);                                                                                                                                   \
    QJS_FUNC_COMP(CLASS, __VA_ARGS__);

#define QJS_FUNCTION_NODCTOR(CLASS, ...)                                                                                                                                 \
  public:                                                                                                                                                                \
    QJS_FUNC_COPY(CLASS, __VA_ARGS__);                                                                                                                                   \
    QJS_FUNC_JSON(CLASS, __VA_ARGS__);                                                                                                                                   \
    QJS_FUNC_COMP(CLASS, __VA_ARGS__);
