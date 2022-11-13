# Magic Config Framework

This README describes the Magic Config C++ framework which aims to automate much of the
boilerplate code that one usually has to write when developing a configuration system.

# Table of Contents
* [Thoughts on Configuration](#Thoughts-on-Configuration)
* [Framework Capabilities](#Framework-Capabilities)
* [Code Organization and Conventions](#Code-Organization-and-Conventions)
* [Examples](#Examples)
* [Requirements](#Requirements)
* [Acknowledgements](#Acknowledgements)

## Thoughts on Configuration

There are certainly a 1000 ways to approach configuration and there is a proliferation
of design choices which a developer might make. This section describes my thoughts on
configuration and what I have found to work well during my years of working on various
projects across different organizations.

Here are the main principles. A config (system):
* Should be easy to work with and understand
* Should be extensible
* Should require minimal effort and minimize the boilerplate code needed
  to consume / process config files
* Should have easy to reproduce semantics so multiple runs of the
  same executable with the same config produce the **exact same** results
* Should aim to be checkpointable, i.e. there should ideally be a step in
  the config process which produces a static config which can optionally
  be saved in a data store for compliance and reproducibility reasons
* Should provide easy validation ideally by the very program it configures to
  avoid the prospect of the validation and ingestion code getting out-of-sync
* Should have a format which easily integrates with external tooling

This framework focuses on configuring arbitrarily complex C++ object hierarchies.
One observation I would like to make is that the JSON standard provides for
easy externalization of arbitrarily complex objects. For example, arbitrarily
complex Python objects and dictionaries are easily serializable to and from JSON.

The main complaint people have when it comes to JSON is that it does not allow
comments and some find it hard to read. It should be further noted that JSON is
a subset of YAML and thus there exists a direct human-readable mapping from YAML
to JSON which in addition supports comments.

The magic_config framework aims to provide the same sort of mapping from an
JSON/YAML config to an arbitrarily complex C++ object as can be found in Python.
The user can choose the format based on their readability/verbosity preferences.

JSON and YAML are further static and thus checkpointable, can easily integrate
with third-party tooling, and are widely accepted formats which are familiar
to most developers!

One of the main issues that remains when choosing these formats is that one
tends to write a lot of boilerplate code to process them, extract the data
within them and load it into the target C++ object. Fear not, magic_config
solves this problem and more! (Spoiler alert: *think validation and extensibility*)

Finally, let me describe my preferred approach to configuration. As you have
already guessed the main intermediary format is JSON/YAML, but how are there
configs generated and maintained?! Well, that is entirely up to you!
While you can certainly hand-craft them, I have found that tedious. Employing the
generative power of a scripting language such as Python to maintain and generate
these intermediary static JSON/YAML configs can go a long way towards unlocking
the power to add smarts and dynamicity to the config generation process, while
still ensuring that a checkpointable artifact is produced that meets many of the
criteria described above! It addition, as previously mentioned, Python dictionaries
are easily convertible to JSON/YAML via one-liners so you can have your cake and
eat it too!

To summarize - write any highly intelligent config generation code in Python,
produce a dictionary object and then just dump it to whichever format you prefer.
Magic config will do the rest! See more in the next section.


## Framework Capabilities
[Back to ToC](#Table-of-Contents)

Magic Config is a C++ config framework which aims to automate JSON or YAML (JSON subset only)
configuration mapping to an arbitrarily complex C++ class/struct hierarchy and enable the user
to avoid writing any traversal and loading code that normally must be written and maintained.

You simply inherit from one of the magic_config types (YamlConfig or JsonConfig), define
a static defineConfigMapping() function to associates key names in the input config with
class members, and then simply call YourClass::load(config) to seamlessly and ***magically***
load the input config ***without writing any boilerplate code***!

See [Thoughts on Configuration](#Thoughts-on-Configuration) for the long version of why this
is a good way to approach configuration!

The framework does all the work to map the contents of a config file to an arbitrarily complex
class definition hierarchy, e.g. a class definition may contain one or more complex objects
(possibly held in STL or other containers). They are recursively mapped by magic_config to sub-sections
(indentation levels) of the input config. The framework supports simple types, composite types
and the most widely used sequence and associative STL containers out-of-the-box. For a good example of
composite types see [Composite Example](#Composite-Example).

Magic Config further offers the ability to validate various aspects of the input config.
There are currently two different mechanisms for validation - property checks and the
optional verify_config() post-validation method which is automatically detected and
called by the framework upon loading the input config into the target object.

The available property checks are:
 * Required     - setting a field as mandatory will cause parsing to fail if it is not present
 * Cardinality  - Calls member.size() and confirms that it is equal to a specified value
 * Range        - Checks that the value is within a given inclusive or exclusive range
 * Action       - User-specified lambda which is attached as a property checker
 * User-defined - User can also inherit & implement the IPropertyCheck interface to add custom properties

Property checks are implemented via a proxy class so you can easily chain them. In the event that
verification or property checks fail, the framework will throw and error.
See [Basic Validation](#Basic-Validation-Example) and [Advanced Validation](#Advanced-Validation-Example)
examples for more info!

The [Composite Example](#Composite-Example) shows the Magic Config's ability to map arbitrarily complex
class hierarchies to the structure of input configs. In addition to that the framework also employs the
power of template traits to seamlessly provide support for third-party containers which have a STL-friendly
interface. See [this example](#Support-for-STL-friendly-Third-Party-Libraries) for more info.

Note that if support lacks for a given type, the framework can also be easily extended to support it!
See [this example](#Support-for-Non-STL-Friendly-Third-Party-Libraries-or-Custom-Types) for more
information on that!

Finally the Magic Config framework is generic. This makes it fairly easy to integrate new third-party
config parsing libraries, e.g. your special JSON parser. To add a new parser library you will need to
adapt magic_config by providing a new set of traits and converters to handle behavior particular to
this new parsing librabry. Currently we support yaml-cpp (which is used to implement YamlConfig) and
folly::dynamic (which is used to implement JsonConfig). Feel free to look at how we have integrated
these two parsers for more info. The main thing to remember, though, is that magic_config,
***by design***, **supports only what can be expressed via a JSON document**!

The framework can be used as a **header-only** library. If the user wishes to compile the tests or
the examples, we currently use the Meson build system and GTest as a test framework. See the
[Requirements](#Requirements) section for more info.

## Code Organization and Conventions
[Back to ToC](#Table-of-Contents)

Top level directories:

```
magic:         Main framework code

dynamic_json:  Support for JSON by way of folly::dynamic
               See: magic_config::dynamic_json::JsonConfig

yaml:          Support for YAML & JSON via yaml-cpp
               See: magic_config::yaml::YamlConfig

examples:      See Examples section for more info

```

**NOTE**: YamlConfig supports both YAML and JSON. It both cases it supports *only the JSON subset of YAML*!
<br>

### Test Naming Convention

The tests for a given component are usually located in {component name}.t.cpp
The test code is usually in the same directory as the component.

An exception is made for top-level user-facing directories where tests are
organized in a test/ subdirectory to let the user focus directly on the interface files.

The examples below are also written as tests, largely as a means to document expected behavior.
All test code can simply be ignored/deleted should you decide to copy any part of the example.

## Examples

The Examples directory offers examples of common usage patterns and shows how the framework
aids the developer in solving issues that might arise. The examples should be examined in the
provided order below as they build on each other. Each example subsection is a link to the
underlying cpp file implementing the example. Make sure you search them for **NOTE** comments
as they highlight important things!

As mentioned in the [Framework Capabilities](#Framework-Capabilities) section,
Magic Config is a generic framework which utilizes traits to configure itself.
We utilize this in the Examples and compile the same example code twice, once for
each of the currently supported config types - YamlConfig and JsonConfig.
See [examples/Setup.hpp](examples/Setup.hpp) for more details.

I have written the examples as GTest programs as that makes them more expressive and
ensures that they remain live code.

#### [Basic Example](examples/Basic.cpp)

The example above shows basic usage. It is the bare minimum use case. The following examples
build on it and showcase further features.

#### [Inheritance Example](examples/Inheritance.cpp)

The example above demonstrates the use case where one or more magic configs are inherited into
the final config.

#### [Basic Validation Example](examples/BasicValidation.cpp)

The example above showcases basic validation. Magic Config offers two different mechanisms for
validation - property checks and optional verify_config post validation method which is
automatically detected and called by the framework upon loading the config into the object.
This example demonstrates both. The following example focuses on more advanced user-defined
validation.

#### [Advanced Validation Example](examples/AdvancedValidation.cpp)

The example above showcases how to attach a lambda as a property check.
For most users this should be more than enough!

For those few brave souls who, armed with strong understanding of C++ templates, want to dig
into the depths of the framework, it is also possible to define your own custom property checks.
This example shows you exactly how while also warning you that lambdas should be preferred unless
you have a really good reason to implement your own property checks.

#### [Composite Example](examples/Composite.cpp)

The example above shows you how you can compose arbitrarily complex types and map them to the underlying
config simply by specifying the key names that should match the various data members. The framework
takes care of traversing the various config levels, finding the appropriate data items and loading them.


#### [Support for STL-friendly Third Party Libraries](examples/ThirdPartyLibSTL.cpp)

The example above (named ThirdPartyLibSTL.cpp) showcases Magic Config framework's ability to
seamlessly support third-party library containers which loosely conform to the STL interface
of similar STL sequence and associative containers. The boost::containers::small_vector here
loosely conforms to the std::vector interface and the framework works out-of-the-box.
The way it does it is by using container traits to determine the type of the container and
generic helper converters for sequences and associative containers to subsequently load the data.
See the example for more details!


#### [Support for Non-STL-Friendly Third Party Libraries or Custom Types](examples/ThirdPartyLibNonSTL.cpp)

The example above (named ThirdPartyLibNonSTL.cpp) shows how to teach the Magic Config framework
to interact with your custom types and third-party containers which do not conform to STL
interfaces. Unlike the prior example, there is a bunch of plumbing we need to do, namely, we need
to specialize the Converter interface for the new type. You will see that it is not particularly
hard especially if you are familiar with the underlying input config library, e.g. yaml-cpp or
folly::dynamic in the case of our example.


## Requirements
[Back to ToC](#Table-of-Contents)

NOTE: The main framework is header-only! The following are required by the framework:

* GCC 10 or higher | C++17 or higher

* yaml-cpp

* Boost CPP libraries (only boost::lexical_cast)

The following are only required for compiling the tests:

* Meson build system  (only if you wish to compile the tests)

* GTest for testing

* Optionally: Facebook/folly if using anything under json/, e.g. JsonConfig


## Acknowledgements
[Back to ToC](#Table-of-Contents)

This framework is loosely inspired by the PCCL (Python-C++ Config Loader) framework.

See https://github.com/jzwinck/pccl for more details.
