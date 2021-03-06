#include <dynaconf/include/Scope.h>
#include <cassert>
#include <iostream>

using namespace dynaconf;

int main( void )
{
	// Create a resolution scope
	//
	auto scope = std::make_shared<Scope>();

	// Define an singleton for class (silly self-referential example).
	//
	set( scope, make_singleton<Scope>( scope ) );
	assert( get<Scope>( scope ) == scope );

	// Create a child scope and modify the definition
	//
	auto child = std::make_shared<Scope>( scope );
	assert( get<Scope>( child ) == scope );
	set( child, make_singleton<Scope>( std::shared_ptr<Scope>{ nullptr } ) );
	assert( get<Scope>( child ) == nullptr );

	// Demo some polymorphic behavior based on resolution scope.
	//
	set( scope, make_singleton<std::string>( std::make_shared<std::string>( "parent scope" ) ) );
	set( child, make_singleton<std::string>( std::make_shared<std::string>( "child scope" ) ) );

	struct MyType { std::string msg; };

	set( scope, make_factory<MyType>( []( const std::shared_ptr<const Scope> & current )
	{
		// This code process the request with the caller's scope, allowing it
		// to use the current effective definitions to build a response.
		//
		auto desc = std::string{ "Instance created using " } + *get<std::string>( current );
		return std::make_shared<MyType>( MyType{ std::move( desc ) } );
	}));

	std::cout << get<MyType>( scope )->msg << std::endl;	// "Instance created using parent scope"
	std::cout << get<MyType>( child )->msg << std::endl;	// "Instance created using child scope"

	// Regular polymorphism works too:
	//
	struct BaseType { 
		virtual const char * name() = 0;
		virtual ~BaseType() {}
	};
	struct TypeA : BaseType {
		virtual const char * name() { return "TypeA"; }
		virtual ~TypeA() {}
	};
	struct TypeB : BaseType { 
		virtual const char * name() { return "TypeB"; }
		virtual ~TypeB() {}
	};

	set( scope, make_singleton<BaseType>( std::make_shared<TypeA>() ) );
	set( child, make_singleton<BaseType>( std::make_shared<TypeB>() ) );

	std::cout << get<BaseType>( scope )->name() << std::endl;	// "TypeA"
	std::cout << get<BaseType>( child )->name() << std::endl;	// "TypeB"

	return 0;
}
