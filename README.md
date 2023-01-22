
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
license and Copyright notices are maintained ["per file"][file scope].

## How to use

Open VS solution, select `Release` configuration and compile this project, then copy `asmformat.exe`
from `Build` directory somewhere where it will be accessible, ex. `PATH`.

Alternatively you can download already built executable from `Releases` section.

- Default encoding, if not specified is `utf8`, to override use `--encoding` option.
- Default tab width, if not specified is 4, to override use `tabwidth` option,\
  note that tab width option also affects spaces.
- By default tabs are used, to use spaces pass `--spaces` option to command line.
- By default line breaks are preserved but you can override with `--linebreaks` option,\
  `--linebreaks` option currently doesn't have any effect on UTF-16 encoded files,
  UTF-16 files are always formatted with `CRLF`
- By default surplus blank lines are removed at the top and at the end of a file, as well as surplus
  blank lines around procedure labels to make them compacted to code.\
  if you whish to replace all surplus blank lines entirely with a single blank line specify
  `--compact` option.

For up to date help and program command line syntax run `asmformat.exe --help`

You have to be careful to specify correct encoding which depends on encoding of your asm sources,\
specifying incorrect encoding may turn your sources into a pile garbage symbols, also formatting
might not works as expected if encoding is not correct.

Also tab width option must be correct, which depends on your editor configuration,
specifying incorrect tab width will produce unexpected results.

Depending on your editor you should configure it so that `asmformat` is executed on demand for
currently opened file.

Here is sample task configuration for `VSCode` which needs to be put into `.vscode\tasks.json` file.

```json
{
	"version": "2.0.0",
	"tasks": [
		{
			"label": "asm: format",
			"detail": "format current file",
			"type": "process",
			"command": "${workspaceFolder}\\asmformat.exe",
			"args": [
				"${file}",
				"--encoding",
				"ansi",
				"--tabwidth",
				"4"
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

Otherwise you can as well run the formatter on command line, ex:

```batch
asmformat.exe .\filename.asm --encoding utf8 --tabwidth 4
```

Any command line argument which doesn't start with `"--"` is interpreted as path to asm file,
this means you can specify multiple files at once.\
If you don't specify full path to file current working directory of `asmformat` is searched.

# Demonstration

The following sample animation demonstrates current rudimentary capabilities:

![Demonstration](/assets/demonstration.gif)

[masm]: https://learn.microsoft.com/en-us/cpp/assembler/masm/microsoft-macro-assembler-reference
[badge license]: https://img.shields.io/static/v1?label=License&message=MIT&color=success&style=plastic
[file scope]: https://softwarefreedom.org/resources/2012/ManagingCopyrightInformation.html#maintaining-file-scope-copyright-notices
