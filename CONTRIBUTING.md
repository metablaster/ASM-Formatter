
# How to contribute

This document provides guidelines on how to contribute code and what sorts of
contributions are acceptable

## General guidelines

Here is a list of introductory material to help you get started.

The following two pages below explain general starting guidelines regarding open source

- [How to contribute to open source][contribute to open source]
- [Open Source Contribution Etiquette][open source etiquette]

First step is to fork a project

- [Forking a repo][Forking a repo]

Next if needed, you might want to set up your `SSH` keys

- [Connecting to GitHub with SSH][github ssh]

Regarding license and Copyright practices adopted by this project see

- [Maintaining file-scope copyright notices][filescope copyright]
- [Requirements under U.S. and E.U. Copyright Law][copyright law]
- [Copyright Notices][copyright notices]

Regarding versioning adopted see

- [Semantic Versioning 2.0.0][semantic versioning]

For quick markdown references

- [Mastering Markdown][markdown mastering]
- [Markdown Style Guide][markdown style]
- [Markdown tables generator][markdown tables]

Language specific

- [C++ Core Guidelines][guidelines]

## Contributing

Contributing is welcome problem however is that there are no well established community guidelines
on how assembly source files should be formatted.

For this reason only the following contributions are acceptable:

- Your formatting implementation must be very basic and minimal because personal coding style
preferences must not dictate how the formatter will work.

- Implementation for other assemblers which should cover everything that is already implemented by
this project.

- Implementation for other operating systems

- Support for more file encodings

- Bugfixes, optimizations, improvements and tests of existing code base

If you wish to do something unorthodox or if you believe some new feature might be useful then
please open a new issue to discuss it first.

Formatting implementation for other assemblers should be a new function rather than mixed with
existing format functions.

## Development environment

By default Visual Studio is used but you feel free to use and contribute your own build configuration

Please do:

1. Compile clean `\W4` including static analysis
2. Get familiar with existing error handling code
3. Review code design, criticize and optimize code

[contribute to open source]: https://opensource.guide/how-to-contribute "How to contribute to open source"
[open source etiquette]: https://tirania.org/blog/archive/2010/Dec-31.html "Open Source Contribution Etiquette"
[Forking a repo]: https://docs.github.com/en/get-started/quickstart/fork-a-repo "Forking a repo"
[github ssh]: https://docs.github.com/en/authentication/connecting-to-github-with-ssh "Connecting to GitHub with SSH"
[filescope copyright]: https://softwarefreedom.org/resources/2012/ManagingCopyrightInformation.html#maintaining-file-scope-copyright-notices "Maintaining file-scope copyright notices"
[copyright law]: http://softwarefreedom.org/resources/2007/originality-requirements.html "Requirements under U.S. and E.U. Copyright Law"
[copyright notices]: https://www.gnu.org/prep/maintain/html_node/Copyright-Notices.html "Copyright Notices"
[semantic versioning]: https://semver.org "Semantic Versioning"
[markdown mastering]: https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax "Visit GitHub"
[markdown style]: https://cirosantilli.com/markdown-style-guide "Visit markdown guide"
[markdown tables]: https://www.tablesgenerator.com/markdown_tables "Visit table generator site"
[guidelines]: https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md "Visit GitHub page"
