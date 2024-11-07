% SHT-UNSUCK(1) Version 1.0 | Sht Manual

NAME
====

**sht unsuck** — Check local directory for non-compliant characters and prompt for automatic rename.

SYNOPSIS
========
| ***sht unsuck*** \[**-f** | **--force**]

DESCRIPTION
===========

Plumbing sht command for renaming filenames to something easier to work with. 

Options
-------

-f, --force

:   forces yes for all renames

COMMANDS
========

```bash
Normalize file 'Normie Name.txt' to Normie_Name.txt?
         [a | Y/n | q]: 
```

(a)ll, (A)ll

:   Confirms renames and sets force flag to true

(y)es, (Y)es

:   Confirms rename

(n)o, (N)o

:   Skips file without renaming

(q)uit, (Q)uit

:   Exits

BUGS
====

See GitHub Issues or report your own: <https://github.com/ben256dev/sht/issues>

AUTHOR
======

Benjamin Blodgett <benjamin@ben256.com>

SEE ALSO
========

**sht**(1)
