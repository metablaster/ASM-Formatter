
# ASM Formatter

This is an **experimental** formatter for assembly source files.\
**NOTE:** please make a backup of your assembly sources before using it!

It's currently designed for [MASM][masm] assembler syntax and at the moment supports the following:

- Consistent formatting of comments
- Formatting inline comments so that all are equally distant from code
- Consistent code indentation
- Removal of leading and trailing white spaces
- Removal of starting and ending surplus blank lines
- Procedures are separated by blank line and compacted
- Optional removal of surplus blank lines
- Conversion from tabs to spaces and vice versa
- Conversion from `LF` to `CRLF` and vice versa

The following file encodings are supported at the moment:

- ANSI
- UTF-8
- UTF-16LE

## License

[![MIT license][badge license]](/LICENSE "View license")

This project `ASM Formatter` is licensed under the `MIT` license.

A small portion of code is not `MIT` licensed or may have their own authors,\
license and Copyright notices are thus maintained ["per file"][file scope].

## How to use

Download or clone this repository, open VS solution, select `Release` configuration and compile project,
then copy `asmformat.exe` from `Build` directory somewhere where it will be accessible, ex. `PATH`.\
Alternatively you can download already built executable from `Releases` section.

**NOTE:** If you whish to compile for an older version of Windows, open `targetver.hpp` header and
uncomment appropriate macros then in addition install VC++ redistributable package for target `MSVC`.

First step is to run `asmformat.exe --help` for an up to date help to learn formatter options.

You have to be careful to specify correct encoding which depends on encoding of your asm sources,\
specifying an incorrect encoding may turn your sources into a pile of garbage symbols, also formatting
might not work properly or may not work at all if incorrect encoding is specified.

Also tab width option must be correct, which depends on your editor configuration,
specifying incorrect tab width will produce unexpected results.

Depending on your editor you should configure it so that `asmformat` is executed on demand for
currently opened file.

Here is a sample task configuration for `VSCode` which needs to be put into `.vscode\tasks.json` file.\
For more information about tasks please refer to [Integrate with External Tools via Tasks][tasks]

```jsonc
{
	"version": "2.0.0",
	"tasks": [
		{
			"label": "asm: format",
			"detail": "format current file",
			"type": "process",
			// TODO: Update path to asmformat
			"command": "${workspaceFolder}\\asmformat.exe",
			"args": [
				// TODO: Run: asmformat --help and specify correct args
				"${file}",
				"--encoding",
				"ansi",
				"--tabwidth",
				"4",
				"--compact",
				"--nologo"
			],
			"presentation": {
				"showReuseMessage": false
			},
			"problemMatcher": []
		}
	]
}
```

Then simply open your assembly source file in `VSCode` and click on menu `Terminal -> Run Task...`\
and choose `asm: format` option which will format currently opened file.

Otherwise you can as well run the formatter on command line, for ex:

```batch
asmformat.exe .\filename.asm --encoding utf8 --tabwidth 4 --compact
```

## Formatter command line syntax

```
[-path] file1.asm [dir\\file2.asm ...] [--directory DIR] [--encoding ansi|utf8|utf16le] [--tabwidth N] [--spaces] [--linebreaks crlf|lf] [--compact] [--version] [--nologo] [--help]
```

Options and arguments mentioned in square brackets `[]` are optional

| Option         | Argument         | Description                                                               |
| -------------- | ---------------- | ------------------------------------------------------------------------- |
| --path         | file path        | Explicitly specify path to file                                           |
| --directory    | directory name   | Specifies directory which to search for *.asm files to format             |
| --encoding     | encoding ID      | Specifies default encoding used to read and write files (default: ansi)   |
| --tabwidth     | positive integer | Specifies tab width used in source files (default: 4)                     |
| --spaces       | none             | Use spaces instead of tabs (by default tabs are used)                     |
| --linebreaks   | linebreak ID     | Perform line breaks conversion (by default line breaks are preserved)     |
| --compact      | none             | Replaces all surplus blank lines with single blank line                   |
| --version      | none             | Shows program version                                                     |
| --nologo       | none             | Suppresses the display of the program banner when the asmformat starts up |
| --help         | none             | Displays up to date detailed help                                         |

**Notes:**

Any command line argument which doesn't start with `"--"` is interpreted as asm file or path to asm file,
this means you can specify multiple files at once.

- `--path` option specifies explicit path to single asm file which may be full path name or file name only.
  If `--path` option is omitted and file name only is specified then in addition to environment's current working
  directory, also working directory of asmformat is searched.
  Otherwise if you specify full path to file name without `--path` the behavior is same.

- `--encoding` option is ignored if file encoding is auto detected, in which case a message is
  printed telling that the option was ignored in favor of actual file encoding.

- The default encoding, if not specified is `ANSI`, to override use `--encoding` option.

- By default line breaks are preserved but you can override with `--linebreaks` option.
  The `--linebreaks` option may also be used to correct inconsistent linebreaks.

- `--linebreaks` option doesn't have any effect on `UTF-16` encoded files, `UTF-16` files are always
  formatted with `CRLF`.

- By default surplus blank lines are removed at the top and at the end of a file,
  as well as surplus blank lines around procedure labels to make them compacted to code.

- If you whish to replace all surplus blank lines entirely with a single blank line specify
  `--compact` option.

- By default tabs are used, to use spaces pass `--spaces` option to command line, whether you'll
  use that option or not depends on whether your sources are formatted with spaces or tabs?

- The default tab width, if not specified is `4`, to override use `--tabwidth` option,\
  note that tab width option also affects spaces, that is, how many spaces are used for tab in
  existing sources?

- If you specify same option more than once, ex by mistake, the last one is used.
  `--path` and `--directory` options can be specified multiple times and all will be processed.

## Demonstration

The following sample animation demonstrates current rudimentary capabilities:

![Demonstration](/assets/demonstration.gif)

[masm]: https://learn.microsoft.com/en-us/cpp/assembler/masm/microsoft-macro-assembler-reference
[badge license]: https://img.shields.io/static/v1?label=License&message=MIT&color=success&style=plastic
[file scope]: https://softwarefreedom.org/resources/2012/ManagingCopyrightInformation.html#maintaining-file-scope-copyright-notices
[tasks]: https://code.visualstudio.com/docs/editor/tasks
