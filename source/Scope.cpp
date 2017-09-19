#include <dynaconf/include/Scope.h>

namespace dynaconf {

	/// Create a scope with reference to parent scopes.
	///
	/// @param parent scope for recursive resolution.
	///
	Scope::Scope( const std::shared_ptr<Scope> & parent )
	: next( parent )
	{}

	/// Resolve the type_index to a definition--users likely want get().
	///
	/// Applies recursive scope resolution.
	///
	/// @param index to resolve.
	/// @return Definition or nullptr.
	///
	std::shared_ptr<Definition> Scope::resolve( const std::type_index & index ) const
	{
		std::unique_lock<std::mutex> lock( mutex, std::try_to_lock );

		const auto result = definitions.find( index );
		if( result == definitions.end() )
		{
			return next ? next->resolve( index ) : std::shared_ptr<Definition>( nullptr );
		}
		else
		{
			return result->second;
		}
	}

	/// Set a definition in this scope--users likely want set().
	///
	/// @param definition to set as r-reference.
	/// @return true on success, false if Class is already defined.
	///
	bool Scope::define( std::shared_ptr<Definition> && definition )
	{
		std::unique_lock<std::mutex> lock( mutex, std::try_to_lock );

		auto result = definitions.emplace( std::piecewise_construct,
			std::forward_as_tuple( definition->index() ),
			std::forward_as_tuple( std::move( definition ) ) );

		return result.second;
	}
}
