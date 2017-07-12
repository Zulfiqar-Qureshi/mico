#ifndef MICO_OBJECT_REFERENCE_H
#define MICO_OBJECT_REFERENCE_H

#include "mico/environment.h"
#include "mico/objects/base.h"
#include "mico/statements.h"
#include "mico/expressions.h"

namespace mico { namespace objects {

    template <>
    class derived<type::REFERENCE>: public typed_base<type::REFERENCE> {
        using this_type = derived<type::REFERENCE>;
    public:

        using sptr = std::shared_ptr<this_type>;
        using value_type = objects::sptr;

        derived(const environment *my_env, value_type val)
            :my_env_(my_env)
            ,value_(unref(val))
        {
            value_->lock_in( my_env_ );
            locked_ = val->locked( );
        }

        ~derived( )
        {
            value_->unlock_in( my_env_ );
        }

        std::string str( ) const override
        {
            std::ostringstream oss;
            oss << "*"  << value( )->str( ) << "";
            return oss.str( );
        }

        const value_type &value( ) const
        {
            return value_;
        }

        value_type &value( )
        {
            return value_;
        }

        static
        objects::sptr unref( objects::sptr obj )
        {
            if( obj->get_type( ) == objects::type::REFERENCE ) {
                auto ret = static_cast<this_type *>(obj.get( ));
                return ret->value( );
            } else {
                return obj;
            }
        }

        void set_value( const environment *my_env, value_type val )
        {
            if( value_ != val ) {
                //// unlock
                value_->unlock_in( my_env_ );

                ///replace lock
                value_ = val;
                my_env_ = my_env;
                value_->lock_in( my_env_ );
                locked_ = val->locked( );
            }
        }

        static
        sptr make( const environment *my_env, value_type val )
        {
            return std::make_shared<this_type>(my_env, val);
        }

        const environment *env( ) const
        {
            return my_env_;
        }

        bool lock_in( const environment *e ) override
        {
            return value_->lock_in( e );
        }

        bool unlock_in( const environment *e ) override
        {
            return value_->unlock_in( e );
        }

        std::uint64_t hash( ) const override
        {
            return value_->hash( );
        }

        bool equal( const base *other ) const override
        {
            return value_->equal( other );
        }

        std::shared_ptr<base> clone( ) const override
        {
            auto res = std::make_shared<this_type>( my_env_, value_->clone( ) );
            return res;
        }

        std::size_t locked( ) const override
        {
            return locked_;
        }

    private:
        const environment  *my_env_;
        value_type          value_;
        std::size_t         locked_ = 0;
    };


}}

#endif // OBJECT_REFERENCE_H
