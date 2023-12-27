import logging
from aiohttp_jinja2 import template

from app.service.auth_svc import for_all_public_methods, check_authorization
from app.utility.base_world import BaseWorld


@for_all_public_methods(check_authorization)
class Iec61850GUI(BaseWorld):
    def __init__(self, services, name, description):
        self.name = name
        self.description = description
        self.services = services

        self.data_svc = services.get("data_svc")
        self.auth_svc = services.get("auth_svc")
        self.log = logging.getLogger("iec61850_gui")

    @template("iec61850.html")
    async def splash(self, request):
        # Collapse abilities by their ability IDs - this fixes issue of duplicates as locate gives executor variants
        abilities = {
            a.ability_id: {
                "name": a.name,
                "tactic": a.tactic,
                "technique_id": a.technique_id,
                "technique_name": a.technique_name,
                "description": a.description.replace("\n", "<br>"),  # nicer display
            }
            for a in await self.data_svc.locate("abilities")
            if await a.which_plugin() == "iec61850"
        }
        abilities = list(abilities.values())

        return dict(name=self.name, description=self.description, abilities=abilities)
