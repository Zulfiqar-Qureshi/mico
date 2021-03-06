#ifndef MICO_EVAL_TABLES_H
#define MICO_EVAL_TABLES_H

#include "mico/tokens.h"
#include "mico/eval/operations/common.h"

namespace mico { namespace eval { namespace operations {

    template <>
    struct operation<objects::type::TABLE> {

        using table_type = objects::impl<objects::type::STRING>;
        using str_type   = objects::impl<objects::type::STRING>;
        using bool_type  = objects::impl<objects::type::BOOLEAN>;
        using error_type = objects::impl<objects::type::FAILURE>;
        using int_type   = objects::impl<objects::type::INTEGER>;
        using prefix     = ast::expressions::prefix;
        using infix      = ast::expressions::infix;
        using index      = ast::expressions::index;

        static
        objects::sptr eval_prefix( prefix *pref, objects::sptr obj )
        {
            common::reference<objects::type::TABLE> ref(obj);
            auto tt = ref.unref( );

            if( pref->token( ) == tokens::type::ASTERISK ) {
                return int_type::make( tt->value( ).size( ) );
            }
            return error_type::make( pref->pos( ),   "Prefix operator '",
                                     pref->token( ), "' is not defined for"
                                                     " table");
        }

        static
        objects::sptr eval_table( environment::sptr env,
                                  objects::sptr lft, objects::sptr rght )
        {
            auto ltable = objects::cast_table(lft);
            auto rtable = objects::cast_table(rght);

            auto res = ltable->clone( );
            auto restabe = objects::cast_table(res);

            for( auto &v: rtable->value( ) ) {
                restabe->insert( env.get( ), v.first, v.second->value( ) );
            }

            return res;
        }

        static
        objects::sptr eval_index( index *idx, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::TABLE> ref(obj);
            auto tab = ref.shared_unref( );

            objects::sptr id = ev( idx->param( ).get( ), env );

            if( common::is_fail( id ) ) {
                return id;
            }

            if( auto res = tab->at( id ) ) {
                return res;
            }

            return error_type::make( idx->param( )->pos( ),
                                     id->str( ),
                          " is not a valid index for the table" );
        }

        static
        objects::sptr eval_infix( infix *inf, objects::sptr obj,
                                  eval_call ev, environment::sptr env  )
        {
            common::reference<objects::type::TABLE> ref(obj);
            obj = ref.shared_unref( );

            objects::sptr right = ev( inf->right( ).get( ), env );
            if( common::is_fail( right ) ) {
                return right;
            }

            if( (inf->token( ) == tokens::type::EQ) ||
                (inf->token( ) == tokens::type::NOT_EQ) ) {
                return common::eval_equal( inf, obj, right );
            }

            switch (right->get_type( )) {
            case objects::type::TABLE:
                if( inf->token( ) == tokens::type::PLUS ) {
                    return eval_table( env, obj, right );
                }
                break;
            default:
                return common::common_infix( inf, obj, right, env );
            }

            return error_type::make(inf->pos( ), "Infix operation ",
                                    obj->get_type( )," '",
                                    inf->token( ), "' ",
                                    right->get_type( ),
                                    " is not defined");
        }

    };

    using table = operation<objects::type::TABLE>;

}}}

#endif // TABLES_H
