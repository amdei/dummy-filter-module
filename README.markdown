[![Build Status](https://travis-ci.org/amdei/dummy-filter-module.svg?branch=master)](https://travis-ci.org/amdei/dummy-filter-module)    [![Code Health](https://landscape.io/github/amdei/dummy-filter-module/master/landscape.svg?style=flat)](https://landscape.io/github/amdei/dummy-filter-module/master)

Name
====

ngx_dummy_filter - Placeholder for real-world nginx filter modules.

*This module is not distributed with the Nginx source.* See [the installation instructions](#installation).

Table of Contents
=================

* [Status](#status)
* [Synopsis](#synopsis)
* [Description](#description)
* [Installation](#installation)
* [TODO](#todo)
* [Bugs and Patches](#bugs-and-patches)
* [Copyright and License](#copyright-and-license)

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
        dummy_filter on;
    }

    location / {
        # proxy_pass/fastcgi_pass/...

        dummy_filter on;
        dummy_filter_types text/plain text/css;
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
    ./configure --add-module=/path/to/dummy-filter-module
```

[Back to TOC](#table-of-contents)


TODO
====

* ?

[Back to TOC](#table-of-contents)


Bugs and Patches
================

Please submit bug reports, wishlists, or patches by

1. creating a ticket on the [GitHub Issue Tracker](https://github.com/amdei/dummy-filter-module/issues),

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

