#ifndef MICO_EXPRESSIONS_STRING_H
#define MICO_EXPRESSIONS_STRING_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::STRING>: public typed_expr<type::STRING> {

        using this_type = impl<type::STRING>;
    public:
        using uptr = std::unique_ptr<this_type>;

        impl(const std::string &val, bool raw)
            :value_(val)
            ,raw_(raw)
        { }

        explicit
        impl(const std::string &val )
            :value_(val)
            ,raw_(false)
        { }

        std::string str( ) const override
        {
            std::string head( raw_ ? "r\"" : "\"" );
            return head + value_ + '"';
        }

        bool is_raw( ) const
        {
            return raw_;
        }

        const std::string &value( ) const
        {
            return value_;
        }

        std::string &value( )
        {
            return value_;
        }

        static
        uptr make( )
        {
            return uptr(new this_type("", false));
        }

        static
        uptr make( bool raw )
        {
            return uptr(new this_type("", raw) );
        }

        void mutate( mutator_type /*call*/ ) override
        {

        }

        bool is_const( ) const override
        {
            return true;
        }

        ast::node::uptr clone( ) const override
        {
            return uptr( new this_type(value_, raw_ ) );
        }

    private:
        std::string value_;
        bool        raw_ = false;
    };

    using string = impl<type::STRING>;

}}}

#endif // STRING_H
