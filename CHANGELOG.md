# Changelog

Here is a list of changes for each of the releases.

## Table of Contents

- [Changelog](#changelog)
  - [Table of Contents](#table-of-contents)
  - [v0.4.0](#v040)
  - [v0.3.0](#v030)

## v0.4.0

- `--encoding` option is ignored if `BOM` is present in source file
- When possible report an error when an invalid encoding is specified
- Report an error when no files to format were specified
- Improved string conversion functions and conversion error reporting
- Added `--version` option to display program version
- Fixed few mistakes with error and exception handling
- Console code page is set depending on `--encoding` option to be able to print data
contents for error reporting

## v0.3.0

- Formatter no longer forces BOM into files since `MASM` doesn't recognize it
- Line breaks `LF` and `CRLF` are preserved instead of forcing `CRLF`
- Added `--linebreaks` option to force specific line breaks
- Added `--compact` option to optionally replace surplus blank lines with single blank line
- Improved how procedures are separated and compacted
- Improved file read\write operations
- Added `--nologo` option to suppress startup banner
- When the `asmformat` runs it will show what options it is using to format file
