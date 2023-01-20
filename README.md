
# ASM Formatter

This is an **experimental** formatter for assembly source files.\
**NOTE:** please make a backup of your assembly sources before using it!

It's currently designed for [MASM][masm] assembler syntax and at the moment supports the following:

- Consistent formatting of comments
- Formatting inline comments so that all are equally distant from code
- Consistent code indentation
- Removal of leading and trailing white spaces
- Removal of starting and ending surplus blank lines
- Procedures are separated (sectioned) by blank line

The following file encodings are supported at the moment:

- ANSI
- UTF-8
- UTF-16
- UTF-16LE

## License

[![MIT license][badge license]](/LICENSE "View license")

This project `ASM Formatter` is licensed under the `MIT` license.

A small portion of code is not `MIT` licensed or may have their own authors,\
license and Copyright notices are maintained **"per file"**.

## How to use

Open VS solution, select `Release` configuration and compile this project, then copy `asmformat.exe`
from `Build` directory somewhere where it will be accessible, ex. `PATH`.

Alternatively you can download already built executables from `Releases` section.

Default encoding, if not specified is `utf8`\
Default tab width, if not specified is 4\
By default tabs are used if `--spaces` is not specified

For help and program command line syntax run `asmformat.exe --help`

You have to be careful to specify correct encoding which depends on encoding of your asm sources,\
specifying incorrect encoding may turn your sources into a pile garbage symbols.\
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
				"utf8",
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

Any command line argument which doesn't start with `--` is considered to be path to asm file.

# Contributing

Contributing is welcome problem however is that there are no well established community guidelines
on how assembly source files should be formatted.

For this reason only the following contributions are acceptable:

- Implementation for other assemblers which should support everything that is already supported by
this project.

- Your implementation must be very basic and minimal because personal coding style preferences must
not dictate how assembly formatter will work.

- Support for more file encodings

- Bugfixes, optimization and improvements of existing functionalities

If you wish to do something unorthodox or if you believe some new feature might be useful then
please open a new issue to discuss it first.

# Demonstration

The following sample animation demonstrates current rudimentary capabilities:

![Demonstration](/assets/presentation.gif)

[masm]: https://learn.microsoft.com/en-us/cpp/assembler/masm/microsoft-macro-assembler-reference
[badge license]: https://img.shields.io/static/v1?label=License&message=MIT&color=success&style=plastic
