#!/usr/bin/python2
import unittest
import luna_utils as luna

API_URL = "com.webos.service.avoutput/video/"

class TestVideoMethods(luna.TestBase):

    def setUp(self):
        luna.call(API_URL + "disconnect", { "sink": "MAIN" })
        luna.call(API_URL + "disconnect", { "sink": "SUB" })
        self.statusSub = luna.subscribe(API_URL + "getStatus", {"subscribe":True})

    def tearDown(self):
        luna.cancelSubscribe(self.statusSub)

    def testConnectDisconnect(self):
        for source, ports in {"VDEC":[0,1], "HDMI":[0,1,2]}.iteritems():
            for port in ports:
                for sink in ["MAIN", "SUB"]:
                    for i in range(3):
                        self.checkLunaCallSuccessAndSubscriptionUpdate(API_URL + "connect",
                                                                     { "outputMode": "DISPLAY", "sink": sink, "source": source, "sourcePort": port },
                                                                     self.statusSub,
                                                                     {"video":[{"sink": sink, "connectedSource": source, "connectedSourcePort": port, "outputMode": "DISPLAY"}]})

                        self.checkLunaCallSuccessAndSubscriptionUpdate(API_URL + "disconnect", { "sink": sink },
                                                                      self.statusSub,
                                                                      {"video": [{"sink": sink, "connectedSource": None}]})

    def testDualConnect(self):
        self.checkLunaCallSuccessAndSubscriptionUpdate(API_URL + "connect",
                                                      {"outputMode": "DISPLAY", "sink": "MAIN", "source": "VDEC", "sourcePort": 0},
                                                      self.statusSub,
                                                      {"video": [{"sink": "MAIN", "connectedSource": "VDEC", "connectedSourcePort": 0, "outputMode": "DISPLAY"}]})

        self.checkLunaCallSuccessAndSubscriptionUpdate(API_URL + "connect",
                                                      {"outputMode": "DISPLAY", "sink": "SUB", "source": "VDEC", "sourcePort": 1},
                                                      self.statusSub,
                                                      {"video": [{"sink": "MAIN", "connectedSource": "VDEC", "connectedSourcePort": 0, "outputMode": "DISPLAY"},
                                                                 {"sink": "SUB", "connectedSource": "VDEC", "connectedSourcePort": 1, "outputMode": "DISPLAY"}]})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "disconnect", {"sink": "MAIN"},
            self.statusSub, {"video":[{"sink": "MAIN", "connectedSource":None}]})
        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "disconnect", {"sink": "SUB"},
            self.statusSub, {"video":[{"sink": "SUB", "connectedSource":None}]})


    def testMute(self):
        for sink in ["MAIN", "SUB"]:
            self.checkLunaCallSuccessAndSubscriptionUpdate(
                API_URL + "connect",
                {"outputMode": "DISPLAY", "sink": sink, "source": "VDEC", "sourcePort": 0}, self.statusSub, {})

            for blank in [False, True]:
                self.checkLunaCallSuccessAndSubscriptionUpdate(
                    API_URL + "blankVideo",
                    {"sink": sink, "blank": blank},
                    self.statusSub,
                    {"video":[{"sink": sink, "muted": blank}]})

    #test different orders of display window and media data

    def testSetDisplayWindowAndMediaData(self):
        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "connect",
            {"outputMode": "DISPLAY", "sink": "MAIN", "source": "VDEC", "sourcePort": 0}, self.statusSub, {})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "display/setDisplayWindow",
            {"sink": "MAIN",
             "fullScreen": False,
             "sourceInput": {"x":10, "y":11, "width":201, "height":202},
             "displayOutput": {"x":12, "y":13, "width":203, "height":204}},
            self.statusSub,
            {"video":[{"sink": "MAIN",
                   "fullScreen": False,
                   "width":0,
                   "height":0,
                   "frameRate":0,
                   "sourceInput": {"x":0, "y":0, "width":0, "height":0}, # no media data yet
                   "displayOutput": {"x":12, "y":13, "width":203, "height":204}
             }]})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "setMediaData",
            {"sink": "MAIN",
             "contentType": "media",
             "frameRate":29.5,
             "width":220,
             "height":221,
             "scanType":"progressive",
             "adaptive": False},
            self.statusSub,
            {"video":[{"sink": "MAIN",
                   "fullScreen": False,
                   "width":220,
                   "height":221,
                   "frameRate":29.5,
                   "sourceInput": {"x":10, "y":11, "width":201, "height":202},
                   "displayOutput": {"x":12, "y":13, "width":203, "height":204}
             }]})

    def testSetMediaDataAndDisplayWindow(self):
        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "connect",
            {"outputMode": "DISPLAY", "sink": "MAIN", "source": "VDEC", "sourcePort": 0}, self.statusSub, {})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "setMediaData",
            {"sink": "MAIN",
             "contentType": "media",
             "frameRate":29.5,
             "width":220,
             "height":221,
             "scanType":"progressive",
             "adaptive": False},
            self.statusSub,
            {"video":[{"sink": "MAIN",
                   "fullScreen": False,
                   "width":220,
                   "height":221,
                   "frameRate":29.5,
                   "sourceInput": {"x":0, "y":0, "width":220, "height":221},
                   "displayOutput": {"x":0, "y":0, "width":0, "height":0}
               }]})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "display/setDisplayWindow",
            {"sink": "MAIN",
             "fullScreen": False,
             "sourceInput": {"x":10, "y":11, "width":201, "height":202},
             "displayOutput": {"x":12, "y":13, "width":203, "height":204}},
            self.statusSub,
            {"video":[{"sink": "MAIN",
                   "fullScreen": False,
                   "width":220,
                   "height":221,
                   "frameRate":29.5,
                   "sourceInput": {"x":10, "y":11, "width":201, "height":202},
                   "displayOutput": {"x":12, "y":13, "width":203, "height":204}
               }]})

    def testSetFullscreen(self):
        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "connect",
            {"outputMode": "DISPLAY", "sink": "MAIN", "source": "VDEC", "sourcePort": 0}, self.statusSub, {})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "setMediaData",
            {"sink": "MAIN",
             "contentType": "media",
             "frameRate":29.5,
             "width":220,
             "height":221,
             "scanType":"progressive",
             "adaptive": False},
            self.statusSub,
            {"video":[{"sink": "MAIN",
                   "fullScreen": False,
                   "width":220,
                   "height":221,
                   "frameRate":29.5,
                   "sourceInput": {"x":0, "y":0, "width":220, "height":221},
                   "displayOutput": {"x":0, "y":0, "width":0, "height":0}
               }]})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "display/setDisplayWindow",
            {"sink": "MAIN",
             "fullScreen": True,
             "sourceInput": {"x":10, "y":11, "width":201, "height":202}},
            self.statusSub,
            {"video":[{"sink": "MAIN",
                   "fullScreen": True,
                   "width":220,
                   "height":221,
                   "frameRate":29.5,
                   "sourceInput": {"x":10, "y":11, "width":201, "height":202},
                   "displayOutput": {"x":0, "y":0, "width":1920, "height":1080}
               }]})

    def testSetCompositing(self):
        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "connect",
            {"outputMode": "DISPLAY", "sink": "MAIN", "source": "VDEC", "sourcePort": 0}, self.statusSub, {})
        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "connect",
            {"outputMode": "DISPLAY", "sink": "SUB", "source": "VDEC", "sourcePort": 1}, self.statusSub, {})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "display/setCompositing",
            {"mainOpacity":20, "subOpacity":31, "mainZOrder": 1, "subZOrder":0},
            self.statusSub, {"video":[{"sink": "MAIN", "opacity":20, "zOrder":1}, {"sink": "SUB", "opacity":31, "zOrder":0}]})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "display/setDisplayWindow",
            {"sink": "MAIN", "fullScreen":True, "opacity":130, "zOrder": 1},
            self.statusSub, {"video":[{"sink": "MAIN", "opacity":130, "zOrder":1}, {"sink": "SUB", "opacity":31, "zOrder":0}]})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "display/setDisplayWindow",
            {"sink": "MAIN", "fullScreen":True, "zOrder": 0},
            self.statusSub, {"video":[{"sink": "MAIN", "opacity":130, "zOrder":0}, {"sink": "SUB", "opacity":31, "zOrder":1}]})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "display/setDisplayWindow",
            {"sink": "SUB", "fullScreen":True, "opacity":230},
            self.statusSub, {"video":[{"sink": "MAIN", "opacity":130, "zOrder":0}, {"sink": "SUB", "opacity":230, "zOrder":1}]})

        self.checkLunaCallSuccessAndSubscriptionUpdate(
            API_URL + "display/setDisplayWindow",
            {"sink": "SUB", "fullScreen":True, "opacity":30, "zOrder": 1},
            self.statusSub, {"video":[{"sink": "MAIN", "opacity":130, "zOrder":0}, {"sink": "SUB", "opacity":30, "zOrder":1}]})

if __name__ == '__main__':
    luna.VERBOSE = False
    unittest.main()