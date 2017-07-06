#ifndef MICO_ENVIROMENT_H
#define MICO_ENVIROMENT_H

#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <memory>

//#include "mico/objects.h"

#include "mico/object_wrapper.h"

namespace mico {

#define DEBUG 0

#if DEBUG
    static int c = 0;
#endif

    class environment: public std::enable_shared_from_this<environment> {

    public:

        using sptr = std::shared_ptr<environment>;
        using wptr = std::weak_ptr<environment>;

        using object_sptr = std::shared_ptr<objects::base>;
        using object_wptr = std::weak_ptr<objects::base>;

        using children_type = std::set<sptr>;
        using wrapper_type = objects::wrapper<objects::base>;
        using data_map = std::map<std::string, wrapper_type>;

    protected:

        struct key { };

    public:

        struct scoped {

            scoped( sptr v )
                :env_(v)
            {
                env_->lock( );
            }

            ~scoped(  )
            {
                if( env_ ) {
                    env_->unlock( );
//                    env_->clear( );
//                    env_->drop( );
                }
            }

            sptr env( )
            {
                return env_;
            }

            void check( object_sptr o )
            {
//                if( o->lock_factor( ) == 0 ) {
//                    o->on_copy( );
//                }
            }

        private:
            sptr env_;
        };

        environment( key )
        {
#if DEBUG
            std::cout << ++c << "\n";
#endif
        }

        environment( sptr env, key )
            :parent_(env)
        {
#if DEBUG
            std::cout << ++c << "\n";
#endif
        }

        ~environment( )
        {
#if DEBUG
            std::cout << --c << "\n";
#endif
        }

        void clear( )
        {
            if( children_.empty( ) ) {
                data_.clear( );
            }
        }

        static
        sptr make( )
        {
            return std::make_shared<environment>( key( ) );
        }

        static
        sptr make( sptr parent )
        {
            auto res = std::make_shared<environment>( parent, key( ) );
            parent->children_.insert(res);
            return res;
        }

        void drop( sptr child )
        {
            children_.erase( child );
            drop( );
        }

        sptr parent( )
        {
            return parent_.lock( );
        }

        void drop( )
        {
            if( !locked( ) ) {
                if( auto p = parent( ) ) {
                    data_.clear( );
                    p->drop( shared_from_this( ) );
                }
            }
        }

        void lock( )
        {
            locked_++;
            auto p = shared_from_this( );
            while((p = p->parent( ))) {
                p->locked_++;
            }
        }

        std::size_t locked( ) const
        {
            return locked_;
        }

        void unlock( std::size_t count )
        {
            if( locked_ ) {
                locked_-=count;
                auto p = shared_from_this( );
                while((p = p->parent( ))) {
                    p->locked_-=count;
                }
            }
        }

        void unlock( )
        {
            if( locked_ ) {
                locked_--;
                auto p = shared_from_this( );
                while((p = p->parent( ))) {
                    p->locked_--;
                }
            }
        }

        sptr find_contains( const std::string &name )
        {
            auto cur = this;
            sptr parent = shared_from_this( );
            while( cur ) {
                auto f = cur->data_.find( name );
                if( f != cur->data_.end( ) ) {
                    return parent;
                } else {
                    parent = cur->parent( );
                    cur = parent.get( );
                }
            }
            return nullptr;
        }

        void set( const std::string &name, object_sptr val )
        {
            data_[name] = val;
        }

        object_sptr get( const std::string &name )
        {
            auto cur = this;
            object_sptr res;
            sptr parent;
            while( cur && !res ) {
                auto f = cur->data_.find( name );
                if( f != cur->data_.end( ) ) {
                    res = f->second.value( );
                } else {
                    parent = cur->parent( );
                    cur = parent.get( );
                }
            }
            return res;
        }

        data_map &data( )
        {
            return data_;
        }

        children_type &children( )
        {
            return children_;
        }

        void introspect( int level = 0 )
        {
            std::string space( level * 2, ' ' );
            std::cout << space << "Locked: "<< locked( ) << "\n";
            for( auto &d: data_ ) {
                auto us = d.second.value( ).use_count( );
                auto lf = d.second.lock_factor( );
                std::cout << space << d.first << ":"
                          << us
                          << " factor: " << lf
                          << "\n";
            }

            for( auto &c: children_ ) {
                auto cl = c;
                if( !cl ) continue;
                std::cout << space << "Child: "
                          << " l: " << cl->locked_
                          << " ";
                cl->introspect( level + 1 );
            }
        }

        std::size_t maximum_level( )
        {
            std::size_t res = 1;
            maximum_level_hide( res );
            return res;
        }

        void GC( )
        {
            for( auto &c: children( ) ) {
                auto cl = c;
                if( !cl ) {
                    continue;
                }
                if( c->maximum_level( ) == 1  ) {
                    cl->unlock( );
                    cl->children( ).clear( );
                    cl->data( ).clear( );
                    //cl->clear( );
                    //c->drop( );
                }
            }
        }

    private:

        void maximum_level_hide( std::size_t &res )
        {
            for( auto &d: data_ ) {

                auto us = static_cast<std::size_t>(d.second
                                                    .value( )
                                                     .use_count( ));
                res = (res > us) ? res : us;
            }
            for( auto &c: children_ ) {
                auto cl = c;
                if( !cl ) {
                    continue;
                }
                auto us = static_cast<std::size_t>(cl.use_count( ));
                res = (res > us) ? res : us;
                cl->maximum_level_hide( res );
            }
        }

        wptr parent_;
        data_map data_;
        children_type children_;
        std::size_t locked_ = 0;
    };
}

#endif // ENVIROMENT_H