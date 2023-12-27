from app.objects.secondclass.c_fact import Fact
from app.objects.secondclass.c_relationship import Relationship
from app.utility.base_parser import BaseParser
import re


WRITE_RE = re.compile("^\w+ succeeded: (\w+)((/\w{4,})(\.\w+)?([\.\w]+)?)?(\[(\w{2})\])? -> (.+)$")


class Parser(BaseParser):
    def parse(self, blob):
        relationships = []
        for match in self.line(blob):
            facts = self._parse_write(match)

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
    def _parse_write(line):
        facts = {}
        m = WRITE_RE.fullmatch(line)
        if not m:
            return facts

        facts["iec61850.device.name"] = m.group(1)
        if facts.get("iec61850.device.name") and m.group(3):
            facts["iec61850.node.name"] = facts["iec61850.device.name"] + m.group(3)
        if facts.get("iec61850.node.name") and m.group(4):
            facts["iec61850.object.name"] = facts["iec61850.node.name"] + m.group(4)
        if facts.get("iec61850.object.name") and m.group(5):
            facts["iec61850.attribute.name"] = facts["iec61850.object.name"] + m.group(5)

        if m.group(7):
            facts["iec61850.attribute.fc"] = m.group(7)
        if m.group(8):
            facts["iec61850.attribute.value"] = m.group(8)

        return facts
