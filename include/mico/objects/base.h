#ifndef MICO_OBJECT_BASE_H
#define MICO_OBJECT_BASE_H

#include <string>
#include <memory>
#include <vector>
#include <algorithm>

#include "mico/ast.h"
#include "mico/types.h"

#if defined(DISABLE_SWITCH_WARNINGS)
#ifdef __clang__
#   pragma clang diagnostic ignored "-Wswitch"
#elif defined __GNUC__
#  pragma GCC diagnostic ignored "-Wswitch"
#endif
#endif

namespace mico {
    class environment;
}

namespace mico { namespace objects {

    enum class type {
        NULL_OBJ    =  0,
        BOOLEAN     =  1,
        INTEGER     =  2,
        FLOAT       =  3,
        STRING      =  4,
        CHARACTER   =  5,
        TABLE       =  6,
        ARRAY       =  7,
        REFERENCE   =  8,
        RETURN      =  9,
        FUNCTION    = 10,
        TAIL_CALL   = 11,
        BUILTIN     = 12,
        FAILURE     = 13,
        QUOTE       = 14,
        MODULE      = 15,
        GENERATOR   = 16,
        INTERVAL    = 17,
        BREAK_OBJ   = 18,
        CONT_OBJ    = 19,
        SSLICE      = 20,
        ASLICE      = 21,
    };

    struct name {
        static
        const char *get( type t )
        {
            switch (t) {
            case type::NULL_OBJ   : return "OBJ_NULL";
            case type::BOOLEAN    : return "OBJ_BOOLEAN";
            case type::INTEGER    : return "OBJ_INTEGER";
            case type::FLOAT      : return "OBJ_FLOAT";
            case type::STRING     : return "OBJ_STRING";
            case type::CHARACTER  : return "OBJ_CHARACTER";
            case type::TABLE      : return "OBJ_TABLE";
            case type::ARRAY      : return "OBJ_ARRAY";
            case type::REFERENCE  : return "OBJ_REFERENCE";
            case type::RETURN     : return "OBJ_RETURN";
            case type::FUNCTION   : return "OBJ_FUNCTION";
            case type::BUILTIN    : return "OBJ_BUILTIN";
            case type::TAIL_CALL  : return "OBJ_TAIL_CALL";
            case type::FAILURE    : return "OBJ_FAILURE";
            case type::QUOTE      : return "OBJ_QUOTE";
            case type::MODULE     : return "OBJ_MODULE";
            case type::GENERATOR  : return "OBJ_GENERATOR";
            case type::INTERVAL   : return "OBJ_INTERVAL";
            case type::BREAK_OBJ  : return "OBJ_BREAK";
            case type::CONT_OBJ   : return "OBJ_CONTINUE";
            case type::SSLICE     : return "OBJ_SLICE_STR";
            case type::ASLICE     : return "OBJ_SLICE_ARR";
            }
            return "<invalid>";
        }
    };

    struct base {

        using hash_type = std::uint64_t;

        virtual ~base( ) = default;
        virtual type get_type( ) const = 0;
        virtual std::string str( ) const = 0;
        virtual std::shared_ptr<base> clone( ) const = 0;
        virtual ast::node::uptr to_ast( tokens::position ) const = 0;

        template <typename T>
        static
        std::size_t hash_combine( std::size_t seed, const T &key )
        {
            std::hash<T> hasher;
            seed ^= hasher(key) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }

        static
        std::size_t hash_combine( std::size_t s, const mico::string &key )
        {
            std::size_t seed = s;
            for( auto &a: key ) {
                seed = hash_combine( seed, a );
            }
            return seed;
        }

        virtual
        hash_type hash( ) const
        {
            std::hash<std::string> h;
            return h( str( ) )  ;
        }

        virtual
        bool equal( const base *other ) const
        {
            return str( ) == other->str( );
        }

        static
        std::uint64_t hash64(uint64_t x)
        {
            std::hash<std::uint64_t> h;
            return h(x);
        }

        virtual
        std::size_t size( ) const
        {
            return 0;
        }

        static
        bool is_container( const base *o )
        {
            return (o->get_type( ) == type::ARRAY)
                || (o->get_type( ) == type::TABLE)
                 ;
        }

        virtual
        const environment *hold( ) const
        {
            return nullptr;
        }

        virtual
        bool mark_in( const environment * )
        {
            return false;
        }

        virtual
        bool unmark_in( const environment * )
        {
            return false;
        }

        virtual
        std::size_t marked( ) const
        {
            return 0;
        }

    };

    template <type TN>
    struct typed_base: public base {
        type get_type( ) const
        {
            return TN;
        }
    };

    template <type>
    class impl;

    using sptr  = std::shared_ptr<base>;
    using wptr  = std::weak_ptr<base>;
    using uptr  = std::unique_ptr<base>;
    using slist = std::vector<sptr>;
    using ulist = std::vector<uptr>;

    template <type ToT>
    inline
    impl<ToT> *cast( base *val )
    {
#if defined(CHECK_CASTS)
        if( ToT != val->get_type( ) ) {
            throw  std::runtime_error( "Bad object* cast" );
        }
#endif
        return static_cast<impl<ToT> *>(val);
    }

    template <type ToT>
    inline
    const impl<ToT> *cast( const base *val )
    {
#if defined(CHECK_CASTS)
        if( ToT != val->get_type( ) ) {
            throw  std::runtime_error( "Bad object* cast" );
        }
#endif
        return static_cast<const impl<ToT> *>(val);
    }

    template <type ToT>
    inline
    std::shared_ptr<impl<ToT> > cast( sptr &val )
    {
#if defined(CHECK_CASTS)
        if( ToT != val->get_type( ) ) {
            throw  std::runtime_error( "Bad shared<object> cast" );
        }
#endif
        return std::shared_ptr<impl<ToT> >(val, cast<ToT>(val.get( ) ) );
    }

    inline
    std::ostream &operator << ( std::ostream &o, const objects::sptr &obj )
    {
        return o << obj->str( );
    }

    inline
    std::ostream &operator << ( std::ostream &o, const objects::base &obj )
    {
        return o << obj.str( );
    }

    inline
    std::ostream &operator << ( std::ostream &o, type t )
    {
        return o << name::get(t);
    }

}}

#endif // OBJECT_BASE_H
