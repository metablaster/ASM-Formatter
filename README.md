
# ASM Formatter

This is an **experimental** formatter for assembly source files which is far from being finished.\
**NOTE:** please make a backup of your assembly sources before using it!

It's currently designed for [MASM][masm] assembler syntax and at the moment supports the following:

- Consistent formatting of comments
- Formatting inline comments so that all are equally distant from code
- Consistent code indentation
- It removes trailing white spaces

The following file encodings are supported at the moment:

- ANSI
- UTF-8
- UTF-16
- UTF-16LE

You have to be careful to specify correct encoding which depends on encoding of your asm sources,\
specifying wrong encoding may turn your sources into a pile garbage symbols.

## How to use

Compile this project and put `asmformat.exe` into your project root directory.\
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
				"utf8"
			],
			"presentation": {
				"showReuseMessage": false
			},
			"problemMatcher": []
		}
	]
}
```

Update configuration above to match correct encoding of source files, for ANSI encoded files you\
can omit `--encoding` option.

Program syntax is simple: `asmformat.exe path\filename.asm [--encoding ansi|utf8|utf16|utf16le]`

Then simply open your assembly source file in `VSCode` and click on menu `Terminal -> Run Task...`\
and choose `asm: format` option which will format currently opened file.

If you're using some other code editor see if it's configurable to run executable against currently
opened file.\
Otherwise you can as well run the formatter on command line, ex:

```batch
asmformat.exe .\filename.asm --encoding utf8
```

# Contributing

Contributing is welcome problem however is that there are no well established community guidelines
on how assembly source files should be formatted.

For this reason only the following contributions are acceptable:

- Implementation for other assemblers which should at a minimum support everything that is already
supported by this project.

- Your implementation must be very basic and minimal because personal coding style preferences must
not dictate how assembly formatter will work.

- Support for more file encodings is the most wanted!

If you wish to do something unorthodox or if you believe some other feature might be useful then
please open a new issue to discuss it first.

# Presentation

The following sample animation demonstrates current rudimentary capabilities:

![Demonstration](/assets/presentation.gif)

[masm]: https://learn.microsoft.com/en-us/cpp/assembler/masm/microsoft-macro-assembler-reference
