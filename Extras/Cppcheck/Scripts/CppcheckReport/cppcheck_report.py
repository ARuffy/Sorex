from typing import List
from typing import Optional
import xml.etree.ElementTree as etree


class Location(object):
    def __init__(self,
                 file: str,
                 line: int,
                 column: Optional[int] = None):
        self.file = file
        self.line = line
        self.column = column

    @staticmethod
    def parse(element: etree.Element):
        column = element.get('column')

        return Location(element.get('file'),
                        int(element.get('line')),
                        column=int(column) if column is not None else None)


class Error(object):
    def __init__(self,
                 id: str,
                 severity: str,
                 msg: str,
                 verbose: str,
                 locations: List[Location]):
        self.id = id
        self.severity = severity
        self.msg = msg
        self.verbose = verbose
        self.locations = locations

    @staticmethod
    def parse(element: etree.Element):
        locations = list()
        location_elements = element.findall('location')
        for location_element in location_elements:
            locations.append(Location.parse(location_element))

        return Error(element.get('id'),
                     element.get('severity'),
                     element.get('msg'),
                     element.get('verbose'),
                     locations)


class Report(object):
    def __init__(self, errors: List[Error]):
        self.errors = errors

    @staticmethod
    def parseXML(xmlPath: str):
        tree = etree.parse(xmlPath)
        root = tree.getroot()

        report_version = root.attrib['version']
        if report_version is None or int(report_version) != 2:
            raise ValueError(
                "Converter only supports CPPCheck XML version 2. Use '--xml-report=2' flag")

        errors = list()
        for error in root.find('errors'):
            errors.append(Error.parse(error))

        return Report(errors)
