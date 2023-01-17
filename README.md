
# ASM-Formatter

This is an **experimental** formatter for assembly source files which is far from finished.

It currently works only for `MASM` syntax and at the moment supports the following:

- Formatting comments
- Formatting inline comments so that all are equally distant from code
- Code indentation

The following file encodings are supported at the moment (but not thoroughly tested):

- ANSI
- UTF-8
- UTF-16
- UTF-16LE

You have to be careful to specify correct encoding which depends on encoding of your asm sources,\
specifying wrong encoding will turn your sources into a pile garbage symbols.

## How to use

Compile and put `asmformat.exe` into your project root directory.\
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

Update configuration above to match correct encoding of source files, for ANSI encoded files you
can omit `--encoding` option.

Program syntax is simple: `asmformat.exe path\filename.asm [--encoding ansi|utf8|utf16|utf16le]`

Then simply open some assembly file in `VSCode` and click on menu `Terminal -> Run Task...` and
choose `asm: format` which will format currently opened file.

If you're using some other code editor see if it's configurable to run executable against currently
opened file.\
Otherwise you can as well run the formatter on command line, ex:

```batch
asmformat.exe .\filename.asm --encoding utf8
```

# Contributing

Contributing is welcome problem however is that there are no well established community guidelines
on how assembly source files should be formatted.

For this reason only the following contributions are welcome:

- Implementation for other assemblers which should at a minimum support everything that is already
supported by this one.

- Your implementation must be very basic and minimal.

If you wish to do something unorthodox please open a new issue to discuss it or alternatively fork
this repo and customize it for yourself however you desire.

# Presentation

The following animation demonstrates current functionality:

![Under construction](/TestFiles/asm.gif)
