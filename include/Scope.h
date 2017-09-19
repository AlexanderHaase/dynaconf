#pragma once
#include <unordered_map>
#include <typeinfo>
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
		std::shared_ptr<Definition> resolve( const std::type_index & index  );

		/// Set a definition in this scope--users likely want set().
		///
		/// @param definition to set as r-reference.
		/// @return true on success, false if Class is already defined.
		///
		bool define( std::shared_ptr<Definition> && definition );

		// Provide default operators.
		//
		Scope( const Scope & ) = default;
		Scope( Scope && ) = default;
		Scope & operator = ( const Scope & ) = default;
		Scope & operator = ( Scope && ) = default;

	protected:
		std::mutex mutex;	///< Thread-safety for definitions.
		std::unordered_map< std::type_index, std::shared_ptr<FactoryBase> > definitions;
		std::shared_ptr<Scope> next;	///< Parent scope or nullptr.
	};


	/// Get a class instance if a definition exists in scope.
	///
	/// @tparam Class to instantiate.
	/// @param scope for resolution.
	/// @return instance or nullptr;
	/// 
	template < typename Class >
	friend std::shared_ptr< Class > ::dynaconf::get( const std::shared_ptr<Scope> & scope )
	{
		auto definition = std::dynamic_pointer_cast< Provider<Class> >( scope.resolve( std::type_index{ typeid(Class) } ) );
		if( definition )
		{
			return definition->instantiate( scope );
		}
		else
		{
			return std::shared_ptr<Class>{ nullptr };
		}
	}


	/// Set a class definition in a scope.
	///
	/// @tparam DefinitionType class type of the definition--likely deduced.
	/// @param scope for definition.
	/// @param definition to set.
	/// @return true on success, false if Class is already defined in this scope.
	/// 
	template < template DefinitionType >
	friend bool ::dynaconf::set( const std::shared_ptr<Scope> & scope, std::shared_ptr< Provider<Class> > definition )
	{
		return scope.define( std::static_pointer_cast<Definition>( definition ) );
	}
}
