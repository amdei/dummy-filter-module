Name
====

ngx_dummy_filter - Placeholder for real-world filter modules.

*This module is not distributed with the Nginx source.* See [the installation instructions](#installation).

Table of Contents
=================

* [Name](#name)
* [Status](#status)
* [Synopsis](#synopsis)
* [Description](#description)
* [Directives](#directives)
    * [replace_filter](#replace_filter)
    * [replace_filter_types](#replace_filter_types)
* [Installation](#installation)
* [Trouble Shooting](#trouble-shooting)
* [TODO](#todo)
* [Community](#community)
    * [English Mailing List](#english-mailing-list)
    * [Chinese Mailing List](#chinese-mailing-list)
* [Bugs and Patches](#bugs-and-patches)
* [Author](#author)
* [Copyright and License](#copyright-and-license)
* [See Also](#see-also)

Status
======

This module is already quite usable though still at the early phase of development
and is considered experimental.

Synopsis
========

```nginx
    location /t {
        default_type text/html;
        echo abc;
        replace_filter 'ab|abc' X;
    }

    location / {
        # proxy_pass/fastcgi_pass/...

        # caseless global substitution:
        replace_filter '\d+' 'blah blah' 'ig';
        replace_filter_types text/plain text/css;
    }

```

Description
===========

This Nginx output filter module tries to noting, just 'process' input.

[Back to TOC](#table-of-contents)

Installation
============

You need rebuild your Nginx like this:

```bash
    ./configure --add-module=/path/to/replace-filter-nginx-module
```

[Back to TOC](#table-of-contents)


TODO
====

* optimize the special case for verbatim substitutions, i.e., `replace_filter <regex> $&;`.
* implement the `replace_filter_skip $var` directive to control whether to enable the filter on the fly.
* reduce the amount of data that has to be buffered for when an partial match is already found.
* recycle the memory blocks used to buffer the pending capture data and "complex values" for replacement.
* allow use of inlined Lua code as the `replacement` argument of the `replace_filter` directive to generate the text to be replaced on-the-fly.

[Back to TOC](#table-of-contents)


Bugs and Patches
================

Please submit bug reports, wishlists, or patches by

1. creating a ticket on the [GitHub Issue Tracker](http://github.com/agentzh/replace-filter-nginx-module/issues),

[Back to TOC](#table-of-contents)

Copyright and License
=====================

This module is licensed under the BSD license.

Copyright (C) 2017, by Andrey Kulikov.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

[Back to TOC](#table-of-contents)

