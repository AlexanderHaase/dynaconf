
# DynaConf #

DynaConf is a configuration framework providing nested-scoped-based resolution of classes. Class resolution is fluent, using template functions to (un)wrap type erasures. Nested scopes function similarly to nested namespaces in python: scopes can hold definitions for arbitrary classes, which dependant scopes are free to override. At the same time scope resolution is more flexible than a pure key-value store: Definitions may reference the invoking scope when resolving a class to resolve other definitions, functioning as argument passing in a factory pattern. The flexibility provided by scope resolution offers a localized, const-correct alternative to global state such as singletons while maintaining seperabitily and testability within an application.

Class resolution adhears to const-correctness so that resolution doesn't become a state machine unto itself: During resolution definitions may query the current scope, but may not modify it. 

```c++
#include <dynaconf/include/Scope.h>

using namespace dynaconf;

/* .... */

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

```

## Motivation ##

DynaConf leverages c++11 to provide a fluent alternative to singletons and global state that promotes separability within a code base. Scopes provide a natural avenue for injecting mocks, proxies, and other debugging tools. Scopes also provide a fluent dialect for delayed and/or localized evaluation since definitions are resolved with access to the current effective scope.

## Options ##

`Options` provide a way of disambiguating multiple potential sources of a definition. Options were created with configuration parsing in mind. Options use text keys to identify a specific variaint of a definition. For command-line parsing, arguments might map to keys:

```sh
--connection=ZMQ`
```

...might correspond to...

```c++
set<Connection>( scope, "ZMQ" options );
```

For JSON, object keys might also find use: 

```js
{ 'Connection': 'ZMQ' }
```

...might correspond to...

```c++
auto scope = std::make_shared<Scope>( parent );
set<Parser>( scope, "Connection", options );
set<Options>( scope, "Connection", options );
auto parser = get<Parser>( scope );
parser->parse( scope, "ZMQ" );

```

Here's a concrete (if contrived) example of `Options`:

```c++
using namespace dynaconf;

/* .... */

struct Person
{
	std::string name;
	unsigned int age;

	static bool make_option( const std::shared_ptr<Options> & options, const std::string & name, unsigned int age )
	{
		return set( options, name, make_singleton<Person>( std::make_shared<Person>( name, age ) ) );
	}
};

auto options = std::make_shared<Options>();

Person::make_option( options, "Fred", 54u );
Person::make_option( options, "Alice", 32u );
Person::make_option( options, "Sally", 5u );

auto scope = std::make_shared<Scope>();

set<Person>( scope, "Alice", options );
```
