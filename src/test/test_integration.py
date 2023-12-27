import platform
import subprocess
import unittest
from pathlib import Path

ROOT_DIR = Path(__file__).parent.parent
EXAMPLE_DIR = ROOT_DIR / "build" / "bin" / "examples"
EXE_DIR = ROOT_DIR / "build" / "bin"

def get_bin_name() -> Path:
    uname = platform.system()
    if uname == "Windows":
        return EXE_DIR / "iec61850_actions.exe"
    elif uname == "Darwin":
        return EXE_DIR / "iec61850_actions_darwin"
    return EXE_DIR / "iec61850_actions"

class TestGet(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        server_exe = EXAMPLE_DIR / "server_example_basic_io"
        if not server_exe.exists():
            raise unittest.SkipTest(f"could not find test server at {server_exe}")
        
        cls.server_process = subprocess.Popen(
            [server_exe], stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )

    @classmethod
    def tearDownClass(cls):
        cls.server_process.terminate()

    def setUp(self):
        self.bin = get_bin_name()
        if not self.bin.exists():
            raise unittest.SkipTest(f"could not find actions binary at {self.bin}")

    def test_get_logical_devices(self):
        args = [self.bin, "get", "logical_devices", "localhost"]
        p = subprocess.run(args, capture_output=True, text=True)
        expected = "simpleIOGenericIO\n"
        self.assertEqual(p.stdout, expected)

    @unittest.skip("slow test, requires timeout")
    def test_get_logical_devices_bad_host(self):
        args = [self.bin, "get", "logical_devices", "10.10.10.10"]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("failed", p.stderr)

    def test_get_logical_nodes(self):
        args = [self.bin, "get", "logical_nodes", "simpleIOGenericIO", "localhost"]
        p = subprocess.run(args, capture_output=True, text=True)
        expected = "".join(
            [
                "simpleIOGenericIO/GGIO1\n",
                "simpleIOGenericIO/LLN0\n",
                "simpleIOGenericIO/LPHD1\n",
            ]
        )
        self.assertEqual(p.stdout, expected)

    def test_get_data_objects(self):
        args = [self.bin, "get", "data_objects", "simpleIOGenericIO/LLN0", "localhost"]
        p = subprocess.run(args, capture_output=True, text=True)
        expected = "".join(
            [
                "simpleIOGenericIO/LLN0.Mod\n",
                "simpleIOGenericIO/LLN0.NamPlt\n",
                "simpleIOGenericIO/LLN0.Beh\n",
                "simpleIOGenericIO/LLN0.Health\n",
            ]
        )
        self.assertEqual(p.stdout, expected)

    def test_get_data_attrs(self):
        args = [
            self.bin,
            "get",
            "data_attributes",
            "simpleIOGenericIO/LLN0.Mod",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        expected = "".join(
            [
                "simpleIOGenericIO/LLN0.Mod.ctlModel[CF]\n",
                "simpleIOGenericIO/LLN0.Mod.q[ST]\n",
                "simpleIOGenericIO/LLN0.Mod.stVal[ST]\n",
                "simpleIOGenericIO/LLN0.Mod.t[ST]\n",
            ]
        )
        self.assertEqual(p.stdout, expected)

    def test_get_value(self):
        args = [
            self.bin,
            "get",
            "value",
            "simpleIOGenericIO/LLN0.NamPlt.vendor",
            "--fc",
            "DC",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        expected = "simpleIOGenericIO/LLN0.NamPlt.vendor: MZ Automation (type: visible-string, fc: DC)\n"
        self.assertEqual(p.stdout, expected)

    def test_get_logical_nodes_bad_fc(self):
        args = [
            self.bin,
            "get",
            "value",
            "simpleIOGenericIO/LLN0.NamPlt.vendor",
            "--fc",
            "XX",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("unable to parse", p.stderr)

    def test_get_logical_nodes_no_fc(self):
        args = [
            self.bin,
            "get",
            "value",
            "simpleIOGenericIO/LLN0.NamPlt.vendor",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("unable to parse", p.stderr)

    def test_get_data_sets(self):
        args = [self.bin, "get", "data_sets", "localhost"]
        p = subprocess.run(args, capture_output=True, text=True)
        expected = "".join(
            [
                "simpleIOGenericIO/LLN0.Events (not deletable)\n",
                "simpleIOGenericIO/LLN0.Events2 (not deletable)\n",
                "simpleIOGenericIO/LLN0.Measurements (not deletable)\n",
            ]
        )
        self.assertEqual(p.stdout, expected)

    def test_get_reports(self):
        args = [self.bin, "get", "reports", "localhost"]
        p = subprocess.run(args, capture_output=True, text=True)
        expected = "".join(
            [
                "Buffered Report: EventsBRCBPreConf01\n",
                "Buffered Report: EventsBRCB01\n",
                "Buffered Report: Measurements01\n",
                "Buffered Report: Measurements02\n",
                "Buffered Report: Measurements03\n",
                "Unbuffered Report: EventsIndexed01\n",
                "Unbuffered Report: EventsIndexed02\n",
                "Unbuffered Report: EventsIndexed03\n",
                "Unbuffered Report: EventsRCBPreConf01\n",
                "Unbuffered Report: EventsRCB01\n",
            ]
        )
        self.assertEqual(p.stdout, expected)


class TestSet(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        server_exe = EXAMPLE_DIR / "server_example_basic_io"
        if not server_exe.exists():
            raise unittest.SkipTest(f"could not find test server at {server_exe}")
    
        cls.server_process = subprocess.Popen(
            [server_exe], stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )

    @classmethod
    def tearDownClass(cls):
        cls.server_process.terminate()

    def setUp(self):
        self.bin = get_bin_name()
        if not self.bin.exists():
            raise unittest.SkipTest(f"could not find actions binary at {self.bin}")

    def test_set_str1(self):
        args = [
            self.bin,
            "set",
            "simpleIOGenericIO/LLN0.NamPlt.vendor",
            "--fc", "DC",
            "--str", "TestVendor",
            "--read",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("TestVendor", p.stdout)

    def test_set_str2(self):
        args = [
            self.bin,
            "set",
            "simpleIOGenericIO/LLN0.NamPlt.vendor[DC]",
            "--str", "FakeVendor",
            "--read",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("FakeVendor", p.stdout)


class TestControl(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        server_exe = EXAMPLE_DIR / "server_example_basic_io"
        if not server_exe.exists():
            raise unittest.SkipTest(f"could not find test server at {server_exe}")
    
        cls.server_process = subprocess.Popen(
            [server_exe], stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )

    @classmethod
    def tearDownClass(cls):
        cls.server_process.terminate()

    def setUp(self):
        self.bin = get_bin_name()
        if not self.bin.exists():
            raise unittest.SkipTest(f"could not find actions binary at {self.bin}")

    def test_control_bool1(self):
        args = [
            self.bin,
            "control",
            "simpleIOGenericIO/GGIO1.SPCSO1",
            "--bool", "true",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("Control succeeded", p.stdout)

    def test_control_bool2(self):
        args = [
            self.bin,
            "control",
            "simpleIOGenericIO/GGIO1.SPCSO1",
            "--bool", "off",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("Control succeeded", p.stdout)

    def test_control_bool3(self):
        args = [
            self.bin,
            "control",
            "simpleIOGenericIO/GGIO1.SPCSO1",
            "--bool", "1",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("Control succeeded", p.stdout)

    def test_control_bool4(self):
        args = [
            self.bin,
            "control",
            "simpleIOGenericIO/GGIO1.SPCSO1",
            "--bool", "test",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("Control failed", p.stdout)

    def test_control_toggle(self):
        args = [
            self.bin,
            "control",
            "simpleIOGenericIO/GGIO1.SPCSO1",
            "--bool", "false",
            "--toggle",
            "localhost",
        ]
        p = subprocess.run(args, capture_output=True, text=True)
        expected = "".join(
            [
                "Control succeeded: simpleIOGenericIO/GGIO1.SPCSO1 -> false\n",
                "Control succeeded: simpleIOGenericIO/GGIO1.SPCSO1 -> true\n",
            ]
        )
        self.assertEqual(p.stdout, expected)

class TestLog(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        server_exe = EXAMPLE_DIR / "server_example_logging"
        if not server_exe.exists():
            raise unittest.SkipTest(f"could not find test server at {server_exe}")
        cls.server_process = subprocess.Popen(
            [server_exe], stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )

        # control to create logged event
        bin = get_bin_name()
        if not bin.exists():
            raise unittest.SkipTest(f"could not find actions binary at {bin}")
        args = [
            bin,
            "control",
            "simpleIOGenericIO/GGIO1.SPCSO1",
            "--bool", "true",
            "localhost",
        ]
        _ = subprocess.run(args, capture_output=True, text=True)

    @classmethod
    def tearDownClass(cls):
        cls.server_process.terminate()

    def setUp(self):
        self.bin = get_bin_name()
        if not self.bin.exists():
            raise unittest.SkipTest(f"could not find actions binary at {self.bin}")
        if not (EXAMPLE_DIR / "log_status.db").exists():
            raise unittest.SkipTest(f"no logs available for test")

    def test_get_log_blocks(self):
        args = [self.bin, "get", "log_blocks", "localhost",]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("simpleIOGenericIO/LLN0.EventLog", p.stdout)

    def test_get_log(self):
        args = [self.bin, "get", "log", "simpleIOGenericIO/LLN0.EventLog", "localhost",]
        p = subprocess.run(args, capture_output=True, text=True)
        self.assertIn("Log Reference: simpleIOGenericIO/LLN0$EventLog", p.stdout)  # not a very complete test

if __name__ == "__main__":
    unittest.main()
