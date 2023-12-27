from app.objects.secondclass.c_fact import Fact
from app.objects.secondclass.c_relationship import Relationship
from app.utility.base_parser import BaseParser
import re


LCB_RE = re.compile("^((\w+)(/\w{4,})\.\w+):$")
VALUE_RE = re.compile("^([\.\/\w]+): (.+)? \(type: ([\w-]+), fc: (\w{2})\)$")


class Parser(BaseParser):
    def parse(self, blob):
        relationships = []
        for match in self.line(blob):
            facts = self._parse_lcb(match)

            # Mute log block value parsing. Creates a large amount of
            # facts with limited value.
            # facts = self._parse_value(match) if not facts else facts

            for mp in self.mappers:
                source = facts.get(mp.source)
                target = facts.get(mp.target)

                # Don't create relationships with None
                if mp.edge and (source == None or target == None):
                    continue

                relationships.append(
                    Relationship(
                        source=Fact(mp.source, source),
                        edge=mp.edge,
                        target=Fact(mp.target, target),
                    )
                )
        return relationships

    @staticmethod
    def _parse_value(line):
        facts = {}
        m = VALUE_RE.fullmatch(line)
        if not m:
            return facts

        facts["iec61850.attribute.name"] = m.group(1)
        facts["iec61850.attribute.value"] = m.group(2)
        if m.group(3) == "integer":
            facts["iec61850.attribute.datatype"] = "int"
        elif m.group(3) == "unsigned":
            facts["iec61850.attribute.datatype"] = "uint"
        elif m.group(3) == "boolean":
            facts["iec61850.attribute.datatype"] = "bool"
        elif m.group(3) == "visible-string":
            facts["iec61850.attribute.datatype"] = "str"
        else:
            facts["iec61850.attribute.datatype"] = m.group(3)
        facts["iec61850.attribute.fc"] = m.group(4)

        return facts

    @staticmethod
    def _parse_lcb(line):
        facts = {}
        m = LCB_RE.fullmatch(line)
        if not m:
            return facts

        facts["iec61850.lcb.name"] = m.group(1)
        facts["iec61850.device.name"] = m.group(2)
        if facts.get("iec61850.device.name") and m.group(3):
            facts["iec61850.node.name"] = facts["iec61850.device.name"] + m.group(3)
        return facts
