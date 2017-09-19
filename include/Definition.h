#pragma once
#include <memory>

namespace dynaconf {

	/// Base class for definition for a class--erases all type information.
	///
	/// Definitions are instance providers for class types. However, at this
	/// level of abstraction, they provide no concrete meaning. Provider
	/// defines the primary interface: instantiate().
	///
	class Definition {
	public:
		/// Virtual destructor for chaining...
		///
		virtual ~Definition( void ) {}

		/// Get type_index of described class.
		///
		/// @return type_index of defined class.
		///
		virtual std::type_index index( void ) const = 0;
	};


	/// Abstract base for a provider for a specific class.
	///
	/// Provides an abstract interface for getting instances of that class.
	///
	/// @tparam Class struct or class provided by this definition.
	///
	template < typename Class >
	class Provider : public Provider {
	public:
		/// Virtual destructor for chaining...
		///
		virtual ~Provider( void ) {}

		/// Provide the expected implementation for subclasses.
		///
		virtual std::type_index index( void ) const { return std::type_index{ typeid( Class ) }; }

		/// Interface for obtaining an instance fo the described class.
		///
		/// The implementation should be thread-safe.
		///
		/// @param scope to use for constructing the instance of the described class.
		/// @return shared pointer to instance of the class.
		///
		virtual std::shared_ptr<Class> instantiate( const std::shared_ptr<Scope> & scope ) = 0;
	};


	/// Provide a singlton definition of a class.
	///
	/// @tparam Class struct or class provided by this definition.
	///
	template < typename Class >
	class Singleton : public Provider<Class> {
	public:
		/// Virtual destructor for chaining...
		///
		virtual ~Singleton( void ) {}

		/// Return the singleton instance.
		///
		/// @param scope ignored.
		/// @return shared pointer to singleton instance.
		///
		virtual std::shared_ptr<Class> instantiate( const std::shared_ptr<Scope> & scope )
		{
			return instance;
		}

		/// Create the singleton definition from a compatible shared pointer.
		///
		/// @param pointer shared pointer of compatible class.
		/// 
		template < typename Implementation > 
		Singleton( const std::shared_ptr<Implementation> & pointer )
		: instance( std::static_pointer_cast<Class>( pointer ) )
		{}

	protected:
		const std::shared_ptr<Class> instance;	///< Instance provided by this singleton.
	};


	/// Syntatic sugar for creating a Singleton
	///
	/// @tparam Class defined by the singleton.
	/// @tparam Implementation a.k.a. class of the instance.
	/// @param instance provided by the singleton.
	/// @return shared pointer to singleton.
	///
	template< typename Class, typename Implementation >
	auto make_singleton( const std::shared_ptr<Implementation> & instance ) -> std::shared_ptr< Singleton<Class> >
	{
		return std::make_shared< Singleton<Class> >( instance );
	}


	/// Factory for instances based on calling a functor.
	///
	/// Provides virtualization wrappers for the functor. The functor is
	/// subclassed to allow for zero-storage optimization.
	///
	/// @tparam Class defined by the singleton.
	/// @tparam Functor class providing new instances given the scope.
	///
	template < typename Class, template Functor >
	class Factory : public Provider<Class>, protected Functor : {
	public:
		/// Virtual destructor for chaining...
		///
		virtual ~Factory( void ) {}

		/// Delegate instance creation to the functor
		///
		virtual std::shared_ptr<Class> instantiate( const std::shared_ptr<Scope> & scope )
		{
			return Functor::operator() ( scope );
		}

		///! Use deduction to forward l- and r-references.
		///
		/// @tparam Initializer deduced type, likely Functor.
		/// @param initializer for Method instance.
		///
		template < typename Initializer>
		Factory( Initializer && initializer  )
		: Functor( std::forward<Initializer>( initializer ) )
		{}
	};


	/// Syntatic sugar for creating a Factory
	///
	/// @tparam Class defined by the singleton.
	/// @tparam Functor that creates instances
	/// @param functor l- or r-reference.
	/// @return shared pointer to factory.
	///
	template< typename Class, template Functor >
	auto make_factory( Functor && functor ) -> std::shared_ptr< Factory< Class, Functor > >
	{
		return std::make_shared< Factory<Class, Functor> >( std::forward<Functor>( functor ) );
	}


	/// Syntatic sugar for creating a Factory that uses new/delete.
	///
	/// @tparam Class defined by the singleton.
	/// @return shared pointer to factory.
	///
	template< typename Class >
	auto make_factory( void )
	{
		return make_factory<Class>( []( const std::shared_ptr<Scope> & ) { return std::make_shared<Class>(); } );
	}
}
