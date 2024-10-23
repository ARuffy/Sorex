from .cppcheck_report import Error
from .cppcheck_report import Location
from .cppcheck_report import Report

import json
import hashlib
from typing import List


class CodeQualityDumper(object):
    categoriesMap = {
        'none': 'Clarity',
                'error': 'Bug Risk',
                'warning': 'Bug Risk',
                'style': 'Style',
                'performance': 'Performance',
                'portability': 'Compatibility',
                'information': 'Clarity',
                'debug': 'Clarity'
    }

    severityMap = {
        'none': 'info',
        'error': 'blocker',
        'warning': 'critical',
        'style': 'minor',
        'performance': 'major',
        'portability': 'minor',
        'information': 'info',
        'debug': 'info'
    }

    @staticmethod
    def toJson(report: Report):
        errors = report.errors
        result = list()
        for errorObj in errors:
            errorSerialized = {
                'type': 'issue',
                'check_name': errorObj.id,
                'description': errorObj.msg,
                'content': {
                    'body': errorObj.verbose
                },
                'categories': (CodeQualityDumper.categoriesMap[errorObj.severity]),
                'severity': CodeQualityDumper.severityMap[errorObj.severity],
            }

            for locationObj in errorObj.locations:
                locationSerialized = {
                    'path': locationObj.file
                }

                if locationObj.column is None:
                    locationSerialized['lines'] = {
                        'begin': locationObj.line
                    }
                else:
                    locationSerialized['lines'] = {
                        'begin': {
                            'line': locationObj.line,
                            'column': locationObj.column
                        }
                    }

                errorSerialized['location'] = locationSerialized
                errorSerialized['fingerprint'] = hashlib.md5(
                    bytes('{0}:{1}:{2}'.format(
                        locationObj.file, locationObj.line, errorObj.id), 'UTF-8')).hexdigest()
                result.append(errorSerialized)

        return json.dumps(result, indent=4)
