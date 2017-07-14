#ifndef MICO_BUILTIN_H
#define MICO_BUILTIN_H

#include "mico/parser.h"
#include "mico/eval/tree_walking.h"

namespace mico {

    class common: public objects::builtin {

        static
        void def_init( environment::sptr )
        { }

        objects::sptr call( objects::slist &params,
                            environment::sptr e) override
        {
            return call_( params, e );
        }

        void init( environment::sptr e ) override
        {
            return init_( e );
        }

    public:

        using call_type = std::function< objects::sptr
                                        (objects::slist &, environment::sptr)>;
        using init_type = std::function<void (environment::sptr)>;

        common( environment::sptr e, call_type c )
            :objects::builtin(e)
            ,init_(&common::def_init)
            ,call_(std::move(c))
        { }

        common( environment::sptr e, init_type i, call_type c )
            :objects::builtin(e)
            ,init_(i ? std::move(i) : &common::def_init)
            ,call_(std::move(c))
        { }

        static
        objects::sptr make( environment::sptr e, call_type c )
        {
            return std::make_shared<common>( e, std::move(c) );
        }

        static
        objects::sptr make( environment::sptr e, init_type i, call_type c )
        {
            return std::make_shared<common>( e, std::move(i), std::move(c) );
        }

        objects::sptr clone( ) const override
        {
            return std::make_shared<common>( env( ), init_, call_ );
        }

    private:
        init_type init_;
        call_type call_;
    };

    struct len {
        objects::sptr operator ( )( objects::slist &params, environment::sptr )
        {
            static const auto line = __LINE__;
            using objects::error;
            using objects::integer;

            if( params.size( ) < 1 ) {
                return error::make( tokens::position(line, 0),
                                    "Not enough actual parameters for 'len'");
            }

            auto dat = params[0];

            if( dat->get_type( ) == objects::type::STRING ) {
                auto s = static_cast<objects::string *>(dat.get( ));
                return integer::make( s->value( ).size( ));
            } else if( dat->get_type( ) == objects::type::ARRAY ) {
                auto s = static_cast<objects::array *>(dat.get( ));
                return integer::make( s->value( ).size( ));
            } else if( dat->get_type( ) == objects::type::TABLE ) {
                auto s = static_cast<objects::table *>(dat.get( ));
                return integer::make( s->value( ).size( ));
            }
            return error::make( tokens::position( line, 0),
                                "Invalid parameter for 'len'");
        }
    };

    struct gc_show {
        gc_show( environment::sptr e )
            :env(e)
        { }

        objects::sptr operator ( )( objects::slist &, environment::sptr )
        {
            if( auto l = env.lock( ) ) {
                l->introspect(  );
            }
            return objects::null::make( );
        }
        environment::wptr env;
    };

    struct copy {
        objects::sptr operator ( )( objects::slist &p, environment::sptr )
        {
            if( p.size( ) ) {
                return p[0]->clone( );
            }
            return objects::null::make( );
        }
    };

    struct puts {
        objects::sptr operator ( )( objects::slist &pp, environment::sptr )
        {
            static const auto line = __LINE__;
            using objects::error;

            std::size_t count = 0;
            for( auto &p: pp ) {
                ++count;
                if( p->get_type( ) == objects::type::STRING ) {
                    auto s = static_cast<objects::string *>(p.get( ));
                    std::cout << s->value( );
                } else if(p->get_type( ) == objects::type::INTEGER ) {
                    auto s = static_cast<objects::integer *>(p.get( ));
                    std::cout << s->value( );
                } else if(p->get_type( ) == objects::type::FLOAT ) {
                    auto s = static_cast<objects::floating *>(p.get( ));
                    std::cout << s->value( );
                } else if(p->get_type( ) == objects::type::BOOLEAN ) {
                    auto s = static_cast<objects::boolean *>(p.get( ));
                    std::cout << std::boolalpha << s->value( );
                } else {
                    return error::make( tokens::position( line, 0),
                                        "Invalid parameter ", count,
                                        " for 'puts': ", p->get_type( ));
                }
            }
            std::cout << std::endl;
            return objects::null::make( );
        }
    };

    struct builtin {
        static
        void init( environment::sptr env )
        {
            env->set( "len",     common::make( env, len { } ) );
            env->set( "puts",    common::make( env, puts { } ) );
            env->set( "copy",   common::make( env, copy { } ) );
            env->set( "gc_show", common::make( env, gc_show(env) ) );
        }
    };

}

#endif // BUILTIN_H
