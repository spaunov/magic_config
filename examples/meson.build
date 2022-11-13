
boost_container_dep = dependency('boost', modules : ['container'], disabler: true)

mc_example_basic_yaml = executable('mc_example_basic_yaml', 'Basic.cpp',
				   include_directories : root_include,
 				   dependencies : [dep_yaml, dep_gtest])

test('mc_example_basic_yaml', mc_example_basic_yaml)


mc_example_basic_json = executable('mc_example_basic_json', 'Basic.cpp',
				   include_directories : root_include,
				   cpp_args : '-DMAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON',
 				   dependencies : [dep_folly, dep_gtest])

test('mc_example_basic_json', mc_example_basic_json)



mc_example_inheritance_yaml = executable('mc_example_inheritance_yaml', 'Inheritance.cpp',
				   include_directories : root_include,
 				   dependencies : [dep_yaml, dep_gtest])

test('mc_example_inheritance_yaml', mc_example_inheritance_yaml)


mc_example_inheritance_json = executable('mc_example_inheritance_json', 'Inheritance.cpp',
				   include_directories : root_include,
				   cpp_args : '-DMAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON',
 				   dependencies : [dep_folly, dep_gtest])

test('mc_example_inheritance_json', mc_example_inheritance_json)



mc_example_basic_validation_yaml = executable('mc_example_basic_validation_yaml', 'BasicValidation.cpp',
					      include_directories : root_include,
 					      dependencies : [dep_yaml, dep_gtest])

test('mc_example_basic_validation_yaml', mc_example_basic_validation_yaml)


mc_example_basic_validation_json = executable('mc_example_basic_validation_json', 'BasicValidation.cpp',
					      include_directories : root_include,
					      cpp_args : '-DMAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON',
 					      dependencies : [dep_folly, dep_gtest])

test('mc_example_basic_validation_json', mc_example_basic_validation_json)



mc_example_advanced_validation_yaml = executable('mc_example_advanced_validation_yaml', 'AdvancedValidation.cpp',
						 include_directories : root_include,
 						 dependencies : [dep_yaml, dep_gtest])

test('mc_example_advanced_validation_yaml', mc_example_advanced_validation_yaml)


mc_example_advanced_validation_json = executable('mc_example_advanced_validation_json', 'AdvancedValidation.cpp',
						 include_directories : root_include,
						 cpp_args : '-DMAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON',
 						 dependencies : [dep_folly, dep_gtest])

test('mc_example_advanced_validation_json', mc_example_advanced_validation_json)


mc_example_composite_yaml = executable('mc_example_composite_yaml', 'Composite.cpp',
				   include_directories : root_include,
 				   dependencies : [dep_yaml, dep_gtest])

test('mc_example_composite_yaml', mc_example_composite_yaml)


mc_example_composite_json = executable('mc_example_composite_json', 'Composite.cpp',
				   include_directories : root_include,
				   cpp_args : '-DMAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON',
 				   dependencies : [dep_folly, dep_gtest])

test('mc_example_composite_json', mc_example_composite_json)



mc_example_thirdPartyLibSTL_yaml = executable('mc_example_thirdPartyLibSTL_yaml', 'ThirdPartyLibSTL.cpp',
				   include_directories : root_include,
 				   dependencies : [dep_yaml, dep_gtest, boost_container_dep])

test('mc_example_thirdPartyLibSTL_yaml', mc_example_thirdPartyLibSTL_yaml)


mc_example_thirdPartyLibSTL_json = executable('mc_example_thirdPartyLibSTL_json', 'ThirdPartyLibSTL.cpp',
				   include_directories : root_include,
				   cpp_args : '-DMAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON',
 				   dependencies : [dep_folly, dep_gtest, boost_container_dep])

test('mc_example_thirdPartyLibSTL_json', mc_example_thirdPartyLibSTL_json)



mc_example_thirdPartyLibNonSTL_yaml = executable('mc_example_thirdPartyLibNonSTL_yaml', 'ThirdPartyLibNonSTL.cpp',
				   include_directories : root_include,
 				   dependencies : [dep_yaml, dep_gtest])

test('mc_example_thirdPartyLibNonSTL_yaml', mc_example_thirdPartyLibNonSTL_yaml)


mc_example_thirdPartyLibNonSTL_json = executable('mc_example_thirdPartyLibNonSTL_json', 'ThirdPartyLibNonSTL.cpp',
				   include_directories : root_include,
				   cpp_args : '-DMAGIC_CONFIG_EXAMPLES_USE_DYNAMIC_JSON',
 				   dependencies : [dep_folly, dep_gtest])

test('mc_example_thirdPartyLibNonSTL_json', mc_example_thirdPartyLibNonSTL_json)
