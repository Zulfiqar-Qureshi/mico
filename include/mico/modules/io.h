#ifndef MICO_MODULES_IO_H
#define MICO_MODULES_IO_H

#include "mico/builtin.h"
#include "mico/builtin/common.h"
#include "mico/objects/module.h"

namespace mico { namespace modules {

    struct io {

        template <bool NL>
        struct put_impl {

            objects::sptr operator ( )( objects::slist &pp, environment::sptr )
            {
                static const auto line = __LINE__;
                using objects::error;

                static auto res = objects::null::make( );

                std::size_t count = 0;
                for( auto &p: pp ) {
                    ++count;
                    if( p->get_type( ) == objects::type::STRING ) {
                        auto s = objects::cast_string(p.get( ));
                        std::cout << s->value( );
                    } else if(p->get_type( ) == objects::type::INTEGER ) {
                        auto s = objects::cast_int(p.get( ));
                        std::cout << s->value( );
                    } else if(p->get_type( ) == objects::type::FLOAT ) {
                        auto s = objects::cast_float(p.get( ));
                        std::cout << s->value( );
                    } else if(p->get_type( ) == objects::type::BOOLEAN ) {
                        auto s = objects::cast_bool(p.get( ));
                        std::cout << std::boolalpha << s->value( )
                                  << std::noboolalpha;
                    } else {
                        return error::make( tokens::position( line, 0),
                                            "Invalid parameter ", count,
                                            " for 'puts': ", p->get_type( ));
                    }
                }
                if( NL ) {
                    std::cout << std::endl;
                }
                return res;
            }
        };

        using puts = put_impl<true>;
        using put  = put_impl<false>;

        static
        void load( environment::sptr &env, const std::string &name = "io" )
        {
            auto mod_env = environment::make(env);
            auto mod = objects::module::make( mod_env, name );
            mod_env->set( "puts", builtin::common::make( mod_env, puts { } ) );
            mod_env->set( "put",  builtin::common::make( mod_env, put { } ) );
            env->set( name, mod );
        }
    };

}}

#endif // IO_H
