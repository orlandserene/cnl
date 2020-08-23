# Contributing

Contributions are more than welcome! Please read this guide to help ensure your
effort has more effect.

## Submitting Issues

Feel free to [add an issue](https://github.com/johnmcfarlane/cnl/issues/) via GitHub or 
[contact me directly](https://github.com/johnmcfarlane)
([cnl.contributing@john.mcfarlane.name](cnl.contributing@john.mcfarlane.name)).

There is no issue template. Please just try to be as clear as possible. Bugs submitted in the form of pull requests with failing tests
are the best way to submit an issue. (See below for details on pull requests.) Alternatively, examples of code using CNL can be shared using Compiler Explorer ([example](https://godbolt.org/z/vWdds3)).

## Pull Requests

Pull requests should be made against the [main branch](https://github.com/johnmcfarlane/cnl).
When writing commit messages, please follow [these guidelines](https://chris.beams.io/posts/git-commit/)
and try to consider how useful your commit message will be when anyone tries to use `git log` or `git blame` to understand your change. Single-commit PRs will be rebased. Multi-commit PRs will be merged.

PRs should achieve one coherent *thing* or enhance the library in on single *way*. Changes in pursuit 
of that one thing should probably be different commits in the same PR. Public-facing APIs should be
accompanied by tests. (See below.)

All commits need pass all tests on all targets. I may need to mirror your branch in _johnmcfarlane/cnl_ before confirming this.

When writing commit messages, assume the *cnl* directory and the `cnl` namespace, e.g.:

> Move _impl::is_fixed_point into its own header
>  
> * _impl/fixed_point/is_fixed_point.h

## Tests

Please use the contents of [src/test](https://github.com/johnmcfarlane/cnl/blob/develop/src/test/) 
as an example of how to write CNL tests. It's a little chaotic in there (sorry) so look at newer 
commits when determining what example to follow. Prefer `static_assert` tests to run-time tests, try
and keep source files small and dedicted to a single library component of combination of 
components. Follow the project directory structure and code style. (See below.)

## Markdown

Being a GitHub project, I lean toward formatting with [GitHub flavored 
markdown](https://github.github.com/gfm/) but [CommonMark](https://commonmark.org/) is great too.

Files are not computer code. Do not format them as such. Use emphasis instead, e.g.: "To use 
`cnl::fixed_point`, include *cnl/fixed_point.h*".

## Directory Structure

### *include* directory

This is the library include directory. Client code is expected to add it to their system search 
paths (e.g. with `-isystem`) and include the files in [include/cnl](include/cnl). The coverall 
header, [include/cnl/all.h](include/cnl/all.h), should contain most top-level, public-facing APIs
and is intended to be included:

```C++
#include <cnl/all.h>
```

The exception is the content of [include/cnl/auxiliary](include/cnl/auxiliary) which contains 
experimental integration (glue) code for other 3rd-party libraries and which must be pulled in
explicitly.

The contents of [include/cnl/_impl](include/cnl/_impl) and off-limits to library users. The rule 
for the *_impl* directory is that if 

## Code Style

### Namespaces

Full qualify identifiers in *src*. The exceptions are `cnl::_impl::identical` and 
`cnl::_impl::assert_same` which are never the subjects of tests. Keep things out of the global 
namespace where possible. Wrap individual compile-time tests in a separate `test_some_feature` as
exemplified throughout most of the test suite.

Minimally qualify identifiers in source files containted under the *include/cnl* directory. The 
exception is when ADL is expected to be a problem.

## Communication

I am happy to try and communicate in any language but I speak only English well enough to develop 
software. So if you have good English, you will get far more sense with me by using it. 
Many developers have to learn the idiosyncracies of the language just to get stuff done.
Most often, they learned American English. It's not my preferred dialect. 
But it's the lingua franca of the computing world.
 
Out of a respect and admiration for those poor people I ask that the content CNL work be done in American English.
I only have to imagine if I the problems they have had had had.
