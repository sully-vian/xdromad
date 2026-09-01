.PHONY: exec build format xinit

exec: ## Execute xdromad
	dune exec xdromad

build: ## Build xdromad
	dune build
	jq 'map(.directory = (env.PWD // ".") + "/" + .directory | .file = (env.PWD // ".") + "/" + .file)' compile_commands.json | sponge compile_commands.json

format: ## Format code
	dune fmt

xinit: ## Run xdromad in xinit (to use in another tty during development)
	xinit /bin/sh -c "xeyes & mpv av://lavfi:mandelbrot & st & picom & exec $(CURDIR)/_build/default/bin/main.exe" -- :1 vt2 > $(CURDIR)/xdromad.log 2>&1
