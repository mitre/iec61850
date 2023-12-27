from app.objects.secondclass.c_fact import Fact
from app.objects.secondclass.c_relationship import Relationship
from app.utility.base_parser import BaseParser
import re


DATA_SET_RE = re.compile("^((\w+)(/\w{4,})\.\w+) \(.+\)$")
MEMBER_RE = re.compile("^(\w+)((/\w{4,})(\.\w+)?([\.\w]+)?)?(\[(\w{2})\])?(: (.+)? \(type: ([\w-]+), fc: (\w{2})\))?$")


class Parser(BaseParser):
    def parse(self, blob):
        relationships = []
        dataset = None
        for match in self.line(blob):
            if facts := self._parse_data_set(match):
                dataset = facts["iec61850.dataset.name"]

            else:
                facts = self._parse_member(match)
                facts["iec61850.dataset.name"] = dataset

            # add member relationship to dataset
            source = None
            target = None
            if "iec61850.dataset.name" in facts:
                source=Fact("iec61850.dataset.name", facts.get("iec61850.dataset.name"))
            if "iec61850.attribute.name" in facts:
                target=Fact("iec61850.attribute.name", facts.get("iec61850.attribute.name"))
            elif "iec61850.object.name" in facts:
                target=Fact("iec61850.object.name", facts.get("iec61850.object.name"))
            
            if source and target:
                relationships.append(
                    Relationship(source=source, edge="has_member", target=target)
                )

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
    def _parse_member(line):
        facts = {}
        m = MEMBER_RE.fullmatch(line)
        if not m:
            return facts

        # data model
        facts["iec61850.device.name"] = m.group(1)
        if facts.get("iec61850.device.name") and m.group(3):
            facts["iec61850.node.name"] = facts["iec61850.device.name"] + m.group(3)
        if facts.get("iec61850.node.name") and m.group(4):
            facts["iec61850.object.name"] = facts["iec61850.node.name"] + m.group(4)
        if facts.get("iec61850.object.name") and m.group(5):
            facts["iec61850.attribute.name"] = facts["iec61850.object.name"] + m.group(5)

        # functional component
        if m.group(7):
            facts["iec61850.attribute.fc"] = m.group(7)
        elif m.group(11):
            facts["iec61850.attribute.fc"] = m.group(11)

        if m.group(9):
            facts["iec61850.attribute.value"] = m.group(9)

        # datatype
        if m.group(10) == "integer":
            facts["iec61850.attribute.datatype"] = "int"
        elif m.group(10) == "unsigned":
            facts["iec61850.attribute.datatype"] = "uint"
        elif m.group(10) == "boolean":
            facts["iec61850.attribute.datatype"] = "bool"
        elif m.group(10) == "visible-string":
            facts["iec61850.attribute.datatype"] = "str"
        else:
            facts["iec61850.attribute.datatype"] = m.group(10)

        return facts

    @staticmethod
    def _parse_data_set(line):
        facts = {}
        m = DATA_SET_RE.fullmatch(line)
        if not m:
            return facts

        facts["iec61850.dataset.name"] = m.group(1)
        facts["iec61850.device.name"] = m.group(2)
        if facts.get("iec61850.device.name") and m.group(3):
            facts["iec61850.node.name"] = facts["iec61850.device.name"] + m.group(3)
        return facts
