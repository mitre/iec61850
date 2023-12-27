from app.objects.secondclass.c_fact import Fact
from app.objects.secondclass.c_relationship import Relationship
from app.utility.base_parser import BaseParser
import re


DATA_MODEL_RE = re.compile("^(\w+)((/\w{4,})(\.\w+)?([\.\w]+)?)?(\[(\w{2})\])?$")


class Parser(BaseParser):
    def parse(self, blob):
        relationships = []
        for match in self.line(blob):
            facts = self._create_data_model(match)

            for mp in self.mappers:
                source = facts.get(mp.source)
                target = facts.get(mp.target)

                # Don't create relationships with None
                if mp.edge and (source == None or target == None):
                    continue

                relationships.append(
                    Relationship(source=Fact(mp.source, source),
                                 edge=mp.edge,
                                 target=Fact(mp.target, target))
                )
        return relationships
    
    @staticmethod
    def _create_data_model(line):
        data_model = {}
        m = DATA_MODEL_RE.fullmatch(line)
        if not m:
            return data_model

        data_model["iec61850.device.name"] = m.group(1)
        if data_model.get("iec61850.device.name") and m.group(3):
            data_model["iec61850.node.name"] = data_model["iec61850.device.name"] + m.group(3)
        if data_model.get("iec61850.node.name") and m.group(4):
            data_model["iec61850.object.name"] = data_model["iec61850.node.name"] + m.group(4)
        if data_model.get("iec61850.object.name") and m.group(5):
            data_model["iec61850.attribute.name"] = data_model["iec61850.object.name"] + m.group(5)

        if m.group(7):
            data_model["iec61850.attribute.fc"] = m.group(7)
        
        return data_model
