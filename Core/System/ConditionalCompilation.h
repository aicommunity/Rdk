//---------------------------------------------------------------------------
// Conditional Compilation for C++20 Features in GCC 11
// Provides fallbacks for missing C++20 features
//---------------------------------------------------------------------------

#ifndef ConditionalCompilationH
#define ConditionalCompilationH

// Feature detection macros for GCC 11
#if __cpp_lib_ranges >= 201911L
    #define RDK_HAS_RANGES 1
#else
    #define RDK_HAS_RANGES 0
#endif

#if __cpp_concepts >= 202002L
    #define RDK_HAS_CONCEPTS 1
#else
    #define RDK_HAS_CONCEPTS 0
#endif

#if __cpp_lib_jthread >= 201911L
    #define RDK_HAS_JTHREAD 1
#else
    #define RDK_HAS_JTHREAD 0
#endif

#if __cpp_lib_enumerate >= 202302L
    #define RDK_HAS_ENUMERATE 1
#else
    #define RDK_HAS_ENUMERATE 0
#endif

#if __cpp_lib_expected >= 202202L
    #define RDK_HAS_EXPECTED 1
#else
    #define RDK_HAS_EXPECTED 0
#endif

// Conditional includes
#if RDK_HAS_RANGES
    #include <ranges>
#endif

#if RDK_HAS_CONCEPTS
    #include <concepts>
#endif

// GCC 11 already has jthread, so no fallback needed

#if !RDK_HAS_EXPECTED
    #include <optional>
    #include <variant>
    
    namespace std {
        // Simple expected implementation for GCC 11
        template<typename T, typename E>
        class expected {
        private:
            std::variant<T, E> data_;
            
        public:
            expected(const T& value) : data_(value) {}
            expected(T&& value) : data_(std::move(value)) {}
            expected(const E& error) : data_(error) {}
            expected(E&& error) : data_(std::move(error)) {}
            
            bool has_value() const { return std::holds_alternative<T>(data_); }
            bool has_error() const { return std::holds_alternative<E>(data_); }
            
            const T& value() const { return std::get<T>(data_); }
            T& value() { return std::get<T>(data_); }
            
            const E& error() const { return std::get<E>(data_); }
            E& error() { return std::get<E>(data_); }
            
            T value_or(const T& default_value) const {
                return has_value() ? value() : default_value;
            }
            
            template<typename F>
            auto transform(F&& f) -> expected<decltype(f(value())), E> {
                if (has_value()) {
                    return f(value());
                } else {
                    return error();
                }
            }
        };
    }
#endif

// Conditional concepts
#if RDK_HAS_CONCEPTS
    namespace RDK {
        template<typename T>
        concept Copyable = std::copyable<T>;
        
        template<typename T>
        concept Movable = std::movable<T>;
        
        template<typename T>
        concept DefaultConstructible = std::default_initializable<T>;
        
        template<typename T>
        concept Destructible = std::destructible<T>;
    }
#else
    // Fallback SFINAE for concepts
    namespace RDK {
        template<typename T>
        using Copyable = typename std::enable_if<std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>>::type;
        
        template<typename T>
        using Movable = typename std::enable_if<std::is_move_constructible_v<T> && std::is_move_assignable_v<T>>::type;
        
        template<typename T>
        using DefaultConstructible = typename std::enable_if<std::is_default_constructible_v<T>>::type;
        
        template<typename T>
        using Destructible = typename std::enable_if<std::is_destructible_v<T>>::type;
    }
#endif

// Conditional ranges
#if RDK_HAS_RANGES
    namespace RDK {
        using std::ranges::begin;
        using std::ranges::end;
        using std::ranges::size;
        using std::ranges::empty;
        
        template<typename R>
        using range_value_t = std::ranges::range_value_t<R>;
        
        template<typename R>
        using range_reference_t = std::ranges::range_reference_t<R>;
    }
#else
    // Fallback for ranges
    namespace RDK {
        using std::begin;
        using std::end;
        using std::size;
        
        template<typename R>
        auto empty(const R& r) -> decltype(r.empty()) {
            return r.empty();
        }
        
        template<typename T, size_t N>
        bool empty(const T (&)[N]) {
            return N == 0;
        }
        
        template<typename R>
        using range_value_t = typename std::iterator_traits<decltype(std::begin(std::declval<R>()))>::value_type;
        
        template<typename R>
        using range_reference_t = decltype(*std::begin(std::declval<R>()));
    }
#endif

// Conditional enumerate
#if RDK_HAS_ENUMERATE
    namespace RDK {
        using std::views::enumerate;
    }
#else
    // Fallback enumerate implementation
    namespace RDK {
        template<typename R>
        class enumerate_view {
        private:
            R range_;
            
        public:
            explicit enumerate_view(R&& r) : range_(std::forward<R>(r)) {}
            
            class iterator {
            private:
                decltype(std::begin(std::declval<R>())) it_;
                size_t index_;
                
            public:
                iterator(decltype(std::begin(std::declval<R>())) it, size_t idx) 
                    : it_(it), index_(idx) {}
                
                auto operator*() const {
                    return std::make_pair(index_, *it_);
                }
                
                iterator& operator++() {
                    ++it_;
                    ++index_;
                    return *this;
                }
                
                bool operator!=(const iterator& other) const {
                    return it_ != other.it_;
                }
            };
            
            iterator begin() const {
                return iterator(std::begin(range_), 0);
            }
            
            iterator end() const {
                return iterator(std::end(range_), 0);
            }
        };
        
        template<typename R>
        auto enumerate(R&& r) {
            return enumerate_view<R>(std::forward<R>(r));
        }
    }
#endif

#endif // ConditionalCompilationH
