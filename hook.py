from app.utility.base_world import BaseWorld
from plugins.iec61850.app.iec61850_gui import Iec61850GUI
from plugins.iec61850.app.iec61850_api import Iec61850API

name = 'iec61850'
description = 'The IEC 61850 plugin for Caldera provides adversary emulation abilities specific to the IEC 61850 series of communication protocols.'
address = '/plugin/iec61850/gui'
access = BaseWorld.Access.RED


async def enable(services):
    app = services.get('app_svc').application
    iec61850_gui = Iec61850GUI(services, name=name, description=description)
    app.router.add_static('/iec61850', 'plugins/iec61850/static/', append_version=True)
    app.router.add_route('GET', '/plugin/iec61850/gui', iec61850_gui.splash)

    iec61850_api = Iec61850API(services)
    # Add API routes here
    app.router.add_route('POST', '/plugin/iec61850/mirror', iec61850_api.mirror)

