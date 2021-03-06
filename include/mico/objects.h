#ifndef MICO_OBJECTS_H
#define MICO_OBJECTS_H

#include "mico/environment.h"

#include "mico/objects/reference.h"
#include "mico/objects/numbers.h"
#include "mico/objects/null.h"
#include "mico/objects/table.h"
#include "mico/objects/functions.h"

#include "mico/objects/array.h"
#include "mico/objects/string.h"
#include "mico/objects/rstring.h"
#include "mico/objects/slices.h"

#include "mico/objects/character.h"
#include "mico/objects/error.h"
#include "mico/objects/boolean.h"
#include "mico/objects/return.h"
#include "mico/objects/base.h"
#include "mico/objects/quote.h"
#include "mico/objects/module.h"
#include "mico/objects/generator.h"
#include "mico/objects/interval.h"
#include "mico/objects/break.h"
#include "mico/objects/continue.h"
#include "mico/objects/infinite.h"
#include "mico/objects/type.h"

namespace mico { namespace objects {

#define MICO_DEFINE_CAST_FUNC( CallPrefix, TypeName )               \
    inline                                                          \
    impl<TypeName> *cast_##CallPrefix( base *val )                  \
    {                                                               \
        return objects::cast<TypeName>(val);                        \
    }                                                               \
    inline                                                          \
    std::shared_ptr<impl<TypeName> > cast_##CallPrefix( sptr val )  \
    {                                                               \
        return objects::cast<TypeName>( val );                      \
    }

    MICO_DEFINE_CAST_FUNC( null,        type::NULL_OBJ  )
    MICO_DEFINE_CAST_FUNC( int,         type::INTEGER   )
    MICO_DEFINE_CAST_FUNC( float,       type::FLOAT     )
    MICO_DEFINE_CAST_FUNC( bool,        type::BOOLEAN   )
    MICO_DEFINE_CAST_FUNC( string,      type::STRING    )
    MICO_DEFINE_CAST_FUNC( rstring,     type::RSTRING   )
    MICO_DEFINE_CAST_FUNC( char,        type::CHARACTER )
    MICO_DEFINE_CAST_FUNC( table,       type::TABLE     )
    MICO_DEFINE_CAST_FUNC( array,       type::ARRAY     )
    MICO_DEFINE_CAST_FUNC( func,        type::FUNCTION  )
    MICO_DEFINE_CAST_FUNC( builtin,     type::BUILTIN   )
    MICO_DEFINE_CAST_FUNC( tail_call,   type::TAIL_CALL )
    MICO_DEFINE_CAST_FUNC( return,      type::RETURN    )
    MICO_DEFINE_CAST_FUNC( error,       type::FAILURE   )
    MICO_DEFINE_CAST_FUNC( ref,         type::REFERENCE )
    MICO_DEFINE_CAST_FUNC( mod,         type::MODULE    )
    MICO_DEFINE_CAST_FUNC( gen,         type::GENERATOR )
    MICO_DEFINE_CAST_FUNC( ival,        type::INTERVAL  )
    MICO_DEFINE_CAST_FUNC( sslice,      type::SSLICE    )
    MICO_DEFINE_CAST_FUNC( aslice,      type::ASLICE    )
    MICO_DEFINE_CAST_FUNC( rslice,      type::RSLICE    )
    MICO_DEFINE_CAST_FUNC( inf,         type::INF_OBJ   )
    MICO_DEFINE_CAST_FUNC( type,        type::TYPE_OBJ  )

#if !defined(DISABLE_MACRO) || !DISABLE_MACRO
    MICO_DEFINE_CAST_FUNC( quote,       type::QUOTE     )
#endif

#undef MICO_DEFINE_CAST_FUNC

}}

#endif // OBJECTS_H
