#ifndef MICO_EXPRESSION_UNQUOTE_H
#define MICO_EXPRESSION_UNQUOTE_H

#include <sstream>
#include "mico/ast.h"
#include "mico/tokens.h"
#include "mico/expressions/detail.h"

namespace mico { namespace ast { namespace expressions {

    template <>
    class detail<type::UNQUOTE>: public typed_expr<type::UNQUOTE> {

        using this_type = detail<type::UNQUOTE>;

    public:

        using uptr = std::unique_ptr<detail>;
        using value_type = std::shared_ptr<ast::node>;

        detail( ast::node::uptr node )
            :node_(node.release( ))
        { }

        std::string str( ) const override
        {
            std::ostringstream oss;

            oss << "unquote("  << node_->str( ) << ")";

            return oss.str( );
        }

        value_type value( ) const
        {
            return node_;
        }

        static
        uptr make( ast::node::uptr node )
        {
            return uptr(new this_type(std::move(node) ) );
        }

        ast::node::uptr reduce( ast::node::reduce_call call ) override
        {
            if( auto uq = call( node_.get( ) ) ) {
                return uq;
            }
            return node_->clone( );
        }

    private:
        value_type node_;
    };

}}}

#endif // QUOTE_H
