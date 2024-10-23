from .cppcheck_report import Error
from .cppcheck_report import Location
from .cppcheck_report import Report

import os
from typing import List
import xml.etree.ElementTree as etree


class JUnitDumper(object):
    @staticmethod
    def toXML(report: Report):
        errors = report.errors
        root = etree.Element('testsuites')
        root.attrib['id'] = str(1)
        if len(errors) == 0:
            root.attrib['name'] = 'Cppcheck result'
            root.attrib['tests'] = str(1)
            root.attrib['failures'] = str(0)
            root.attrib['time'] = str(1)

            test_suite = etree.SubElement(root,
                                          'testsuite',
                                          id=str(1),
                                          name='Cppcheck errors',
                                          tests=str(1),
                                          failures=str(0),
                                          time=str(1))

            etree.SubElement(test_suite,
                             'testcase',
                             id=str(1),
                             name='Cppcheck success',
                             time=str(1))
        else:
            root.attrib['name'] = 'Cppcheck result'
            root.attrib['tests'] = str(len(errors))
            root.attrib['failures'] = str(len(errors))
            root.attrib['time'] = str(len(errors))

            test_suite = etree.SubElement(root,
                                          'testsuite',
                                          id=str(1),
                                          name='Cppcheck errors',
                                          tests=str(len(errors)),
                                          failures=str(len(errors)),
                                          time=str(len(errors)))

            test_case_id = 1
            for error in errors:
                if len(error.locations) == 0:
                    test_case_name = '[{0}] {1}'.format(error.id,
                                                        error.msg)
                    test_case = etree.SubElement(test_suite,
                                                 'testcase',
                                                 id=str(test_case_id),
                                                 name=test_case_name,
                                                 time=str(1))

                    failure = etree.SubElement(test_case,
                                               'failure',
                                               message=error.verbose,
                                               type='WARNING')
                    failure.text = 'WARNING: {}\nCategory: {}\nMessage: {}'.format(error.id,
                                                                                   error.severity,
                                                                                   error.verbose)
                else:
                    for location in error.locations:
                        test_case_name = '[{0}] {1}:{2}'.format(
                            error.id, location.file, location.line)

                        test_case = etree.SubElement(test_suite,
                                                     'testcase',
                                                     id=str(test_case_id),
                                                     name=test_case_name,
                                                     time=str(1))

                        failure = etree.SubElement(test_case,
                                                   'failure',
                                                   message=error.verbose,
                                                   type='WARNING')

                        failure.text = 'WARNING: {0}\nCategory: {1}\nMessage: {2}\nFile: {3}\nLine: {4}'.format(
                            error.id, error.severity, error.verbose, location.file, location.line)

        return etree.tostring(root, encoding='UTF-8', method='xml').decode()
