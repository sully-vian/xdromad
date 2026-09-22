(lang dune 3.24)

(name xdromad)

(generate_opam_files true)

(source
 (github username/reponame))

(authors "Author Name <author@example.com>")

(maintainers "Maintainer Name <maintainer@example.com>")

(license LICENSE)

(documentation https://url/to/documentation)

(pin
  (url "git+https://github.com/sully-vian/x11.git")
  (package (name x11)))

(package
 (name xdromad)
 (synopsis "A short synopsis")
 (description "A longer description")
 (depends ocaml x11)
 (tags
  ("add topics" "to describe" your project)))

; See the complete stanza docs at https://dune.readthedocs.io/en/stable/reference/dune-project/index.html
