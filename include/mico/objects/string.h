#ifndef MICO_OBJECTS_STRING_H
#define MICO_OBJECTS_STRING_H

#include <string>
#include <sstream>
#include "mico/objects/base.h"
#include "mico/expressions/string.h"

namespace mico { namespace objects {

    template <>
    class impl<type::STRING>: public typed_base<type::STRING> {

        using this_type = impl<type::STRING>;

    public:

        static const type type_value = type::STRING;

        using sptr = std::shared_ptr<this_type>;
        using value_type = std::string;

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "\"" << value( ) << "\"";
            return oss.str( );
        }
        impl( value_type val )
            :value_(std::move(val))
        { }
        const value_type &value( ) const
        {
            return value_;
        }
        value_type &value( )
        {
            return value_;
        }

        hash_type hash( ) const override
        {
            std::hash<std::string> h;
            return h(value_);
        }

        static
        sptr make( value_type val )
        {
            return std::make_shared<this_type>( std::move(val) );
        }

        bool equal( const base *other ) const override
        {
            if( other->get_type( ) == get_type( ) ) {
                auto o = static_cast<const this_type *>( other );
                return o->value( ) == value( );
            }
            return false;
        }

        objects::sptr clone( ) const override
        {
            return std::make_shared<this_type>( value_ );
        }

        ast::node::uptr to_ast( tokens::position pos ) const override
        {
            using ast_type = ast::expressions::impl<ast::type::STRING>;
            return ast::node::make<ast_type>( pos, value( ) );
        }

    private:
        value_type value_;

    };

    using string = impl<type::STRING>;

}}

#endif // STRING_H
