
test_sample_dir = magic_config_root + '/yaml/tests'

message('******** Setting test dir for config/yaml test to: ' + test_sample_dir)

yaml_config_test = executable('yaml_config_test', 'yaml_config.t.cpp',
			      include_directories : root_include,
 			      dependencies : [dep_yaml, dep_gtest])

test('yaml_config_test', yaml_config_test, workdir : test_sample_dir)



convert_yaml_test = executable('convert_yaml_test', 'convert_yaml.t.cpp',
			       include_directories : root_include,
 			       dependencies : [dep_yaml, dep_gtest])

test('convert_yaml_test', convert_yaml_test, workdir : test_sample_dir)
