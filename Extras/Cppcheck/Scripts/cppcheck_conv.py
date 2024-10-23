#!/usr/bin/env python

import sys

req_version = (3, 7)
cur_version = sys.version_info

if cur_version >= req_version:
    from Impl import CppCheckReportConvert

    sys.exit(CppCheckReportConvert().run())
else:
    print('You Python interpreter too old. At least version 3.7.0 is needed.')
    sys.exit(-1)
