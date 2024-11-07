% SHT-STATUS(1) Version 1.0 | Sht Manual

NAME
====

**sht status** — Show which files are tracked.

SYNOPSIS
========
| ***sht status***

DESCRIPTION
===========

Shows which files in the immediate directory are tracked. Directories are listed as untracked. Recursion is not supported currently. If a .sht directory is not found or is incomplete, **sht-init**(1) will need to be ran before-hand. The command also reports filenames with non-compliant characters. If 'sucky' filenames are found, the status will not be printed until files are renamed manually or with **sht-unsuck**(1).

Options
-------

~~-r, --recursive~~

:   ~~shows file status for all files in subtree by recursively checking directories~~

BUGS
====

See GitHub Issues or report your own: <https://github.com/ben256dev/sht/issues>

AUTHOR
======

Benjamin Blodgett <benjamin@ben256.com>

SEE ALSO
========

**sht**(1), **sht-unsuck**(1)
