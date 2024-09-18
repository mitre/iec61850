from app.utility.base_world import BaseWorld
from plugins.iec61850.app.iec61850_svc import Iec61850Service

name = 'iec61850'
description = 'The IEC 61850 plugin for Caldera provides adversary emulation abilities specific to the IEC 61850 series of communication protocols.'
address = '/plugin/iec61850/gui'
access = BaseWorld.Access.RED


async def enable(services):
    iec61850_svc = Iec61850Service(services, name, description)
    app = services.get('app_svc').application
    app.router.add_route('GET', '/plugin/iec61850/gui', iec61850_svc.splash)
    app.router.add_route('GET', '/plugin/iec61850/data', iec61850_svc.plugin_data)