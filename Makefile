.PHONY: exec build

exec:
	dune exec xdromad

build:
	dune build
	jq 'map(.directory = (env.PWD // ".") + "/" + .directory | .file = (env.PWD // ".") + "/" + .file)' compile_commands.json | sponge compile_commands.json

format:
	dune fmt
