
test_sample_dir = magic_config_root + '/dynamic_json/tests'

message('******** Setting test dir for config/dynamic_json test to: ' + test_sample_dir)

convert_json_test = executable('convert_json_test', 'convert_json.t.cpp',
			       include_directories : root_include,
 			       dependencies : [dep_folly, dep_gtest])

test('convert_json_test', convert_json_test, workdir : test_sample_dir)



json_config_test = executable('json_config_test', 'json_config.t.cpp',
			      include_directories : root_include,
  			      dependencies : [dep_folly, dep_gtest])

test('json_config_test', json_config_test, workdir : test_sample_dir)
