import os, pathlib, re, sys

print(sys.argv)

development = (sys.argv[1] == 'true')

annotations = {
	'VOLT_DEVELOPMENT_MODULE_LOAD_CALLBACK': 'volt::modules::_internal::register_development_module_load_callback({id});',
	'VOLT_DEVELOPMENT_MODULE_UNLOAD_CALLBACK': 'volt::modules::_internal::register_development_module_unload_callback({id});',
	'VOLT_COMPONENT': 'volt::ecs::_internal::register_component<{id}>("{id}");',
	'VOLT_SYSTEM': 'volt::ecs::_internal::register_system<{id}>("{id}");',
	'VOLT_ASSET': 'volt::assets::_internal::register_type<{id}>("{id}");'
}

headers_to_include = []
statements_to_insert = []

for i in range(4, len(sys.argv)):
	header_path = os.path.relpath(sys.argv[i], sys.argv[2]) # argv[2] is package root
	header_path = '/'.join(pathlib.Path(header_path).parts[1:])

	lines = open(sys.argv[i], 'r').readlines()
	tokens = []
	if_level = 0
	development_section_if_level = -1

	# Preprocessor TODO: Get list of definitions from CMake and use them for a proper preprocessor implementation
	for line in lines:
		line = line.strip()
		if line.startswith('//'):
			continue

		if not line.startswith('#'):
			if development_section_if_level == -1 or development:
				tokens = tokens + re.findall(r'\w+|[^\w\s]', line)
			continue

		if line.startswith('#ifdef VOLT_DEVELOPMENT') or line.startswith('#if defined(VOLT_DEVELOPMENT)'):
			development_section_if_level = if_level

		if line.startswith('#endif'):
			if_level = if_level - 1

			if if_level == development_section_if_level:
				development_section_if_level = -1
		elif line.startswith('#if'):
			if_level = if_level + 1

	brace_level = 0
	namespace_stack = dict()
	statement_count = len(statements_to_insert)

	token_index = 0
	while token_index < len(tokens):
		token = tokens[token_index]
		token_index = token_index + 1

		if token == '{':
			brace_level = brace_level + 1
		elif token == '}':
			brace_level = brace_level - 1

			if brace_level in namespace_stack:
				del namespace_stack[brace_level]
		elif token == 'namespace':
			namespace_accumulator = ''

			while token_index < len(tokens):
				token = tokens[token_index]
				token_index = token_index + 1

				if token == '{':
					break
				namespace_accumulator = namespace_accumulator + token

			namespace_stack[brace_level] = namespace_accumulator
			brace_level = brace_level + 1
		elif token in annotations:
			annotation = token
			identifier = ''

			while token_index < len(tokens):
				token = tokens[token_index]
				token_index = token_index + 1

				if token == '{' or token == ':' or token == '(':
					if token == '{':
						brace_level = brace_level + 1
					break
				identifier = token

			identifier = '::'.join(namespace_stack.values()) + ('::' if len(namespace_stack) > 0 else '') + identifier
			statements_to_insert.append(annotations[annotation].format(id=identifier))
	
	diff = len(statements_to_insert) - statement_count
	if diff != 0:
		print('[Volt] Found {} annotation{} in: {}'.format(diff, 's' if diff != 1 else '', header_path))
		headers_to_include.append(header_path)

lines = []

lines.append('#include <volt/volt.hpp>')
lines.append('')

for header in headers_to_include:
	lines.append('#include <{}>'.format(header))

lines.append('')
lines.append('extern "C" {')
lines.append('')
lines.append('VOLT_API_EXPORT void volt_module_main() {')

for statement in statements_to_insert:
	lines.append('\t' + statement)

lines.append('}')
lines.append('')
lines.append('}')

os.makedirs(os.path.dirname(sys.argv[3]), exist_ok=True)
open(sys.argv[3], 'w').writelines([line + '\n' for line in lines])
