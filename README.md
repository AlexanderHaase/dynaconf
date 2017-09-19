
# DynaConf #

DynaConf is a configuration framework providing nested-scoped-based(like python namespaces) resolution of classes. Class resolution is fluent, using template functions to (un)wrap type erasures:

```c++
#include <dynaconf/include/Scope.h>

using namespace dynaconf;

// Create a resolution scope
//
auto scope = std::make_shared<Scope>();

// Define an singleton for class (silly self-referential example).
//
set( scope, make_singleton( scope ) );
assert( get<Scope>( scope ) == scope );

// Create a child scope and modify the definition
//
auto child = std::make_shared<Scope>( scope );
assert( get<Scope>( child ) == scope );
set( child, make_singleton<Scope>( nullptr ) );
assert( get<Scope>( child ) == nullptr );

// Demo some polymorphic behavior based on resolution scope.
//
set( scope, make_singleton<std::string>( "parent scope" ) );
set( child, make_singleton<std::string>( "child scope" ) );

struct MyType { std::string msg; };

set( scope, make_factory<MyType>( [] ( const std::shared_ptr<Scope> & current )
{
	// This code process the request with the caller's scope, allowing it
	// to use the current effective definitions to build a response.
	//
	auto desc = std::string{ "Instance created using " } + get<std::string>( current );
	return std::make_shared<MyType>( MyType{ std::move( desc ) } );
});

std::cout << get<MyType>( scope )->msg << std::endl;	// "Instance created using parent scope"
std::cout << get<MyType>( child )->msg << std::endl;	// "Instance created using child scope"

// Regular polymorphism works too:
//
struct BaseType { virtual const char * name() = 0 };
struct TypeA : BaseType { virtual const char * name() { return "TypeA"; } };
struct TypeB : BaseType { virtual const char * name() { return "TypeB"; } };

set( scope, make_factory<BaseType,TypeA>() );
set( child, make_singleton<BaseType>( std::make_shared<TypeB>() ) );

std::cout << get<BaseType>( scope )->name() << std::endl;	// "TypeA"
std::cout << get<BaseType>( child )->name() << std::endl;	// "TypeB"

```

## Motivation ##

DynaConf leverages c++11 to provide a fluent alternative to singletons, class implementation maps, and configuration lookup. Scopes maintain separability while providing equivalent functionality, facilitating testing and traceability by reducing dependence on global state. Scopes create a natural venue for injecting mocks, proxies, and other debugging tools. Scopes also provide a fluent dialect for delayed and/or localized evaluation since definitions are resolved with access to the current effective scope.
