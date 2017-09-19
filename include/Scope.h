#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>
#include <dynaconf/include/Definition.h>

namespace dynaconf {

	/// Scope for defining and resolving provider definitions.
	///
	/// Scopes function as a recursive data structure for resolving type
	/// definitions. In practice, definitions should be type providers.
	/// However, scopes operate through complete type erasure. Fluent usage
	/// revolves around the get<>() and set<>() template functions(template
	/// functions have much less awkward syntax than template methods).
	///
	/// TODO: const-correctness? 
	///
	class Scope {
	public:
		/// Accessor for the parent scope.
		///
		std::shared_ptr<Scope> parent();

		/// Create a scope with reference to parent scopes.
		///
		/// @param parent scope for recursive resolution.
		///
		Scope( const std::shared_ptr<Scope> & parent = std::shared_ptr<Scope>{ nullptr } );

		/// Resolve the type_index to a definition--users likely want get().
		///
		/// Applies recursive scope resolution.
		///
		/// @param index to resolve.
		/// @return Definition or nullptr.
		///
		std::shared_ptr<Definition> resolve( const std::type_index & index  ) const;

		/// Set a definition in this scope--users likely want set().
		///
		/// @param definition to set as r-reference.
		/// @return true on success, false if Class is already defined.
		///
		bool define( std::shared_ptr<Definition> && definition );

		/// Set a definition in this scope--users likely want set().
		///
		/// @param definition to set as l-reference.
		/// @return true on success, false if Class is already defined.
		///
		inline bool define( const std::shared_ptr<Definition> & definition ) { return define( std::shared_ptr<Definition>{ definition } ); }

		// Provide default operators.
		//
		Scope( const Scope & ) = default;
		Scope( Scope && ) = default;
		Scope & operator = ( const Scope & ) = default;
		Scope & operator = ( Scope && ) = default;

	protected:
		mutable std::mutex mutex;	///< Thread-safety for definitions.
		std::unordered_map< std::type_index, std::shared_ptr<Definition> > definitions;
		std::shared_ptr<Scope> next;	///< Parent scope or nullptr.
	};


	/// Get a class instance if a definition exists in scope.
	///
	/// @tparam Class to instantiate.
	/// @param scope for resolution.
	/// @return instance or nullptr;
	/// 
	template < typename Class >
	std::shared_ptr< Class > get( const std::shared_ptr<const Scope> & scope )
	{
		// reinterpret_pointer_cast would be more appropriate, though not available.
		//
		auto definition = std::dynamic_pointer_cast< Provider<Class> >( scope->resolve( std::type_index{ typeid(Class) } ) );
		if( definition )
		{
			return definition->instantiate( scope );
		}
		else
		{
			return std::shared_ptr<Class>{ nullptr };
		}
	}


	/// Get a class instance if a definition exists in scope.
	///
	/// Const-correctness wrapper: more important to be correct than 
	/// overhead-free.
	///
	/// @tparam Class to instantiate.
	/// @param scope for resolution.
	/// @return instance or nullptr;
	/// 
	template < typename Class >
	std::shared_ptr< Class > get( const std::shared_ptr<Scope> & scope )
	{
		return get<Class>( std::const_pointer_cast<const Scope>( scope ) );
	}


	/// Set a class definition in a scope.
	///
	/// @tparam DefinitionType class type of the definition--likely deduced.
	/// @param scope for definition.
	/// @param definition to set.
	/// @return true on success, false if Class is already defined in this scope.
	/// 
	template < typename DefinitionType >
	bool set( const std::shared_ptr<Scope> & scope, std::shared_ptr< DefinitionType > definition )
	{
		return scope->define( std::static_pointer_cast<Definition>( definition ) );
	}
}
