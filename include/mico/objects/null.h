#ifndef MICO_OBJECTS_NULL_H
#define MICO_OBJECTS_NULL_H

#include "mico/objects/base.h"

namespace mico { namespace objects {
    template <>
    class impl<type::NULL_OBJ>: public typed_base<type::NULL_OBJ> {
        using this_type = impl<type::NULL_OBJ>;
    public:

        static const type type_value = type::NULL_OBJ;
        using sptr = std::shared_ptr<this_type>;
        std::string str( ) const override
        {
            return "null";
        }
        static
        sptr make( )
        {
            static auto val = std::make_shared<this_type>( );
            return val;
        }

        hash_type hash( ) const override
        {
            return 0;
        }

        bool equal( const base *o ) const override
        {
            return (o->get_type( ) == get_type( ));
        }

        objects::sptr clone( ) const override
        {
            return make( );
        }

        ast::node::uptr to_ast( tokens::position /*pos*/ ) const override
        {
            return nullptr;
        }

    };

    using null = impl<type::NULL_OBJ>;

}}

#endif // NULL_H
