#ifndef MICO_EXPRESSION_EIFELSE_H
#define MICO_EXPRESSION_EIFELSE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/impl.h"
#include "mico/expressions/list.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class impl<type::IFELSE>: public typed_expr<type::IFELSE> {

        using this_type = impl<type::IFELSE>;
    public:

        using scope_type  = ast::expressions::impl<ast::type::LIST>;
        using scope_value = scope_type::uptr;
        using uptr = std::unique_ptr<impl>;

        struct node {

            explicit
            node( expression::uptr val )
                :cond(std::move(val))
                ,body(scope_type::make_scope( ))
            { }

            node( ) = default;
            node( node && ) = default;
            node &operator = ( node && ) = default;

            node ( node & ) = delete;
            node &operator = ( node & ) = delete;

            expression::uptr cond;
            expression::uptr body;
        };

        using if_list = std::vector<node>;

        impl<type::IFELSE>( bool unless )
            :unless_(unless)
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;

            bool first = true;
            for( auto &f: general_ ) {
                oss << (first ? (unless_ ? "unless " : "if ") : " elif " )
                    << "(" << f.cond->str( ) << ") ";
                oss << "{\n" << f.body->str( ) << "\n}";
                first = false;
            }
            if( alt_ ) {
                oss << " else {\n" << alt_->str( ) << "\n}";
            }
            return oss.str( );
        }

        if_list &ifs( )
        {
            return general_;
        }

        expression::uptr &alt( )
        {
            return alt_;
        }

        const expression::uptr &alt( ) const
        {
            return alt_;
        }

        static
        uptr make( bool unless )
        {
            return uptr(new this_type(unless) );
        }

        void mutate( mutator_type call ) override
        {
            for( auto &g: general_ ) {
                ast::expression::apply_mutator( g.cond, call );
                ast::expression::apply_mutator( g.body, call );
            }
            if( alt_ ) {
                ast::expression::apply_mutator( alt_, call );
            }
        }

        bool is_unless( ) const noexcept
        {
            return unless_;
        }

        bool is_const( ) const override
        {
            for( auto &g: general_ ) {
                if( !g.cond->is_const( ) || !g.body->is_const( ) ) {
                    return false;
                }
            }
            if( alt_ ) {
                return alt_->is_const( );
            }
            return true;
        }

        ast::node::uptr clone( ) const override
        {
            uptr res(new this_type(unless_));
            for( auto &g: general_ ) {
                node next;
                next.cond = expression::call_clone( g.cond );
                next.body = expression::call_clone( g.body );
                res->general_.emplace_back( std::move(next) );
            }
            if( alt_ ) {
                res->alt_ = expression::call_clone( alt_ );
            }
            return ast::node::uptr( std::move( res ) );
        }

    private:
        if_list          general_;
        expression::uptr alt_;
        bool             unless_ = false;
    };

    using ifelse = impl<type::IFELSE>;
}}}

#endif // EIFELSE_H
