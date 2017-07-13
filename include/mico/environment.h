#ifndef MICO_ENVIROMENT_H
#define MICO_ENVIROMENT_H

#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <memory>

#include "mico/objects/base.h"
#include "mico/objects/reference.h"

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
        using obj_reference = objects::derived<objects::type::REFERENCE>;

        using data_map = std::map<std::string, obj_reference::sptr>;

    protected:

        struct key { };

    public:

        struct scoped {

            scoped( sptr v )
                :env_(v)
            {
                if( env_ ) {
                    env_->lock( );
                }
            }

            ~scoped(  )
            {
                if( env_ ) {
                    env_->unlock( );
                }
            }

            sptr env( )
            {
                return env_;
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
            data_.clear( );
            children_.clear( );
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

        const sptr parent( ) const
        {
            return parent_.lock( );
        }

        void drop( )
        {
            if( !locked_ ) {
                auto p = parent( );
                if( p ) {
                    data_.clear( );
                    p->drop( shared_from_this( ) );
                }
            }
        }

        static
        const environment *common_parent( const environment *l,
                                          const environment *r )
        {
            while( l && !r->is_parent( l ) ) {
                l = l->parent( ).get( );
            }
            return l;
        }

        std::size_t is_parent( const environment *pp ) const
        {
            auto c = this;
            std::size_t count = 0;
            while( c ) {
                ++count;
                if( c == pp ) {
                    return count;
                }
                c = c->parent_.lock( ).get( );
            }
            return 0;
        }

        void lock( )
        {
            locked_++;
        }

        std::size_t locked( ) const
        {
            return locked_;
        }

        void unlock( )
        {
            if( locked_ == 0  ) {
                std::cout << "";
                std::cout << "zero unlock!\n";
            }
            locked_--;
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
            data_[name] = obj_reference::make( this, val );
        }

        object_sptr get( const std::string &name )
        {
            auto cur = this;
            object_sptr res;
            sptr parent;
            while( cur && !res ) {
                auto f = cur->data_.find( name );
                if( f != cur->data_.end( ) ) {
                    res = f->second->value( );
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
            for( auto &d: data_ ) {
                std::cout << space << d.first
                          << " => " << d.second->value( )
                          << "\n";
            }

            for( auto &c: children_ ) {
                auto cl = c;
                if( !cl ) continue;
                std::cout << space
                          << "Child: " << cl->locked_
                          << "\n";
                cl->introspect( level + 1 );
            }
        }

        void GC( )
        {
            auto b = children( ).begin( );
            auto e = children( ).end( );
            while( b != e ) {
                auto lck = (*b)->locked( );
                if( 0 == lck ) {
                    (*b)->data( ).clear( );
                    (*b)->children( ).clear( );
                    b = children( ).erase( b );
                } else {
                    //(*b)->GC( );
                    ++b;
                }
            }
            return;
        }

    private:

        wptr            parent_;
        children_type   children_;
        data_map        data_;
        std::size_t     locked_ = 0;
    };
}

#endif // ENVIROMENT_H
