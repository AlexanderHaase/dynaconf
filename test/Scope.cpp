#include <catch.hpp>
#include <dynaconf/include/Scope.h>

template < typename Type>
struct TestDefinition : dynaconf::Definition {
	virtual ~TestDefinition() {}
	virtual std::type_index index( void ) const { return std::type_index{typeid(Type)}; }
};

struct TestType {};

SCENARIO( "scopes should allow definition and resolution of definitions" )
{
	GIVEN( "a scope and a definition" )
	{
		auto definition = std::shared_ptr<dynaconf::Definition>( new TestDefinition<TestType>{} );
		auto scope = std::make_shared<dynaconf::Scope>();

		THEN( "the definition should not be initially defined" )
		{
			REQUIRE( scope->resolve( definition->index() ) == nullptr );
		}

		THEN( "the definition should be definable, then resolvable and immutable" )
		{
			REQUIRE( scope->define( definition ) );
			REQUIRE_FALSE( scope->define( definition ) );
			REQUIRE( scope->resolve( definition->index() ) == definition );
		}
	}

	GIVEN( "multiple dependent scopes" )
	{
		auto definition = std::shared_ptr<dynaconf::Definition>( new TestDefinition<TestType>{} );
		auto replacement = std::shared_ptr<dynaconf::Definition>( new TestDefinition<TestType>{} );
		auto scope = std::make_shared<dynaconf::Scope>();
		auto child = std::make_shared<dynaconf::Scope>( scope );

		THEN( "recursive resolution should propagate" )
		{
			
			REQUIRE( scope->resolve( definition->index() ) == nullptr );
			REQUIRE( child->resolve( definition->index() ) == nullptr );
	
			REQUIRE( scope->define( definition ) );
			REQUIRE( scope->resolve( definition->index() ) == definition );
			REQUIRE( child->resolve( definition->index() ) == definition );
		}

		THEN( "child definitions should override parent definitions" )
		{
			REQUIRE( scope->define( definition ) );
			REQUIRE( child->define( replacement ) );
			REQUIRE( scope->resolve( definition->index() ) == definition );
			REQUIRE( child->resolve( definition->index() ) == replacement );
		}
	}
}

SCENARIO( "the Singleton class should provide a single return value" )
{
	GIVEN( "a scope and a singleton" )
	{
		auto scope = std::make_shared<dynaconf::Scope>();
		auto child = std::make_shared<dynaconf::Scope>( scope );

		THEN( "the singleton should be registerable" )
		{
			REQUIRE( dynaconf::set( scope, dynaconf::make_singleton<TestType>( std::make_shared<TestType>() ) ) );
		}

		THEN( "the return value shouldn't vary" )
		{
			REQUIRE( dynaconf::set( scope, dynaconf::make_singleton<TestType>( std::make_shared<TestType>() ) ) );
			REQUIRE( dynaconf::get<TestType>( child ) == dynaconf::get<TestType>( scope ) );
		}
	}
}

SCENARIO( "the Factory class should allow for scope based construction" )
{
	GIVEN( "dependent scopes, a factory, and sentinel values" )
	{
		auto scope = std::make_shared<dynaconf::Scope>();
		auto child = std::make_shared<dynaconf::Scope>( scope );
		auto other = std::make_shared<dynaconf::Scope>( scope );

		auto scopeValue = std::make_shared<TestType>();
		auto childValue = std::make_shared<TestType>();
		auto factory = dynaconf::make_factory<TestType>( [&]( const std::shared_ptr<const dynaconf::Scope> & current )
		{
			if( current == scope )
			{
				return scopeValue;
			}
			else if( current == child )
			{
				return childValue;
			}
			else
			{
				return std::shared_ptr<TestType>{ nullptr };
			}
		});

		THEN( "the factory should conform to scope based definitions" )
		{
			REQUIRE( dynaconf::set( scope, factory ) );
			REQUIRE( dynaconf::get<TestType>( scope ) == scopeValue );
			REQUIRE( dynaconf::get<TestType>( child ) == childValue );
			REQUIRE( dynaconf::get<TestType>( other ) == nullptr );
		}
	}
}
