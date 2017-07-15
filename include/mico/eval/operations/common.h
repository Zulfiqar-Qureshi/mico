#ifndef MICO_EVAL_COMMON_OPERATIONS_H
#define MICO_EVAL_COMMON_OPERATIONS_H

#include "mico/eval/operation.h"
#include "mico/tokens.h"
#include "mico/objects.h"
#include "mico/expressions.h"

namespace mico { namespace eval { namespace operations {

    struct common {

        using float_type    = objects::floating;
        using value_type    = objects::integer;
        using error_type    = objects::error;
        using bool_type     = objects::boolean;
        using builtin_type  = objects::builtin;
        using function_type = objects::function;

        using prefix = ast::expressions::prefix;
        using infix  = ast::expressions::infix;

        template<objects::type T>
        struct reference {

            using derive_type = objects::derived<T>;
            using shared_derive = std::shared_ptr<derive_type>;

            reference( objects::sptr o )
                :value_(o)
            { }

            objects::base *raw( )
            {
                return value_.get( );
            }

            objects::sptr shared_raw( )
            {
                return value_;
            }

            derive_type *unref( )
            {
                if( value_->get_type(  ) == objects::type::REFERENCE ) {
                    auto rw = objects::cast<objects::type::REFERENCE>(raw( ));
                    return objects::cast<T>( rw->value( ).get( ) );
                } else  {
                    return objects::cast<T>( value_.get( ) );
                }
            }

            shared_derive shared_unref( )
            {
                if( value_->get_type(  ) == objects::type::REFERENCE ) {
                    auto rw = objects::cast_ref( value_ );
                    return objects::cast<T>( rw->value( ) );
                } else  {
                    return objects::cast<T>( value_ );
                }
            }

        private:
            objects::sptr value_;
        };


        static
        objects::sptr common_infix( infix *inf,
                                    objects::sptr left, objects::sptr right,
                                    environment::sptr env )

        {
            switch (right->get_type( )) {
            case objects::type::BUILTIN:
                if(inf->token( ) == tokens::type::BIT_OR) {
                    return eval_builtin( inf, left, right, env);
                }
                break;
            case objects::type::FUNCTION:
                if(inf->token( ) == tokens::type::BIT_OR) {
                    return eval_func( inf, left, right, env);
                }
                break;
            }
            return error_type::make(inf->pos( ), "Infix operation ",
                                    left->get_type( )," '",
                                    inf->token( ), "' ",
                                    right->get_type( ),
                                    " is not defined");
        }


        static
        objects::sptr eval_builtin( infix * /*inf*/,
                                    objects::sptr obj, objects::sptr call,
                                    environment::sptr /*env*/ )
        {
            objects::slist par { obj };
            auto func = objects::cast_builtin(call.get( ));
            auto call_env = environment::make( func->env( ) );
            return objects::tail_call::make( call, std::move(par), call_env );
        }

        static
        objects::sptr eval_func( infix *inf,
                                 objects::sptr obj, objects::sptr call,
                                 environment::sptr /*env*/ )
        {
            auto func = objects::cast_func(call.get( ));
            auto call_env = environment::make( func->env( ) );
            if( func->params( ).size( ) != 1 ) {
                return error_type::make(inf->pos( ),
                                        "Invalid parameters count. ",
                                        "Must be 1");
            }

            for( auto &p: func->params( ) ) {
                if( p->get_type( ) != ast::type::IDENT ) {
                    return error_type::make( inf->pos( ),
                                             "Invalid parameter type.",
                                             p->get_type( ));
                }
                call_env->set( p->str( ), obj );
            }
            return objects::tail_call::make( call, call_env );
        }

    };


}}}

#endif // COMMON_OPERATIONS_H
