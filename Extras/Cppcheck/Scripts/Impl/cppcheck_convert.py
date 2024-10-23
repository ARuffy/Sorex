from CppcheckReport import Report
from CppcheckReport import JUnitDumper
from CppcheckReport import CodeQualityDumper

import argparse

class CppCheckReportConvert(object):
    def __init__(self):
        self._parser = argparse.ArgumentParser(
            description='Convert all files at directory folder to UTF8')
        self._parser.add_argument('-f', '--format', help="Output format. Could be 'junit' or 'codequality'",
                                  required=True,
                                  type=str)
        self._parser.add_argument('cppcheck_xml', type=str)
        self._args = self._parser.parse_args()

    def run(self):
        try:
            report = Report.parseXML(self._args.cppcheck_xml)

            if self._args.format == 'junit':
                print(JUnitDumper.toXML(report))
            elif self._args.format == 'codequality':
                print(CodeQualityDumper.toJson(report))
            else:
                print("Unsupported format '{0}'".format(self._args.format))

            return 0 if len(report.errors) == 0 else 1

        except ValueError as e:
            print(e)
            return 1
