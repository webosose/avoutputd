#!/usr/bin/python2
import unittest
import luna_utils as luna

API_URL = "com.webos.service.avoutput/audio/"

class TestVideoMethods(luna.TestBase):

    def setUp(self):
        luna.call(API_URL + "disconnect", { "sink": "MAIN", "source":"ADEC", "sourcePort":0})
        luna.call(API_URL + "disconnect", { "sink": "SUB", "source":"ADEC", "sourcePort":1 })
        self.statusSub = luna.subscribe(API_URL + "getStatus", {"subscribe":True})

    def tearDown(self):
        luna.cancelSubscribe(self.statusSub)

    def testConnectDisconnect(self):
        for port in [0, 1]:
            for sink in ["MAIN"]:
                for i in range(3):
                    self.checkLunaCallSuccessAndSubscriptionUpdate(API_URL + "connect",
                                                                   { "outputMode": "tv_speaker", "audioType":"media" ,"sink": sink, "source": "ADEC", "sourcePort": port },
                                                                   self.statusSub,
                                                                   {"audio":[{"sink": sink, "source":"ADEC", "sourcePort": port, "outputMode": "tv_speaker", "audioType":"media"}]})

                    self.checkLunaCallSuccessAndSubscriptionUpdate(API_URL + "disconnect", { "sink": sink, "source":"ADEC", "sourcePort":port },
                                                                   self.statusSub,
                                                                   {})

    def testMute(self):
        for sink in ["MAIN"]:
            for source in ["ADEC"]:
                self.checkLunaCallSuccessAndSubscriptionUpdate(
                    API_URL + "connect",
                    { "outputMode": "tv_speaker", "audioType":"media" ,"sink": sink, "source": source, "sourcePort": 0 },
                    self.statusSub, {})

                for blank in [False, True]:
                    self.checkLunaCallSuccessAndSubscriptionUpdate(
                        API_URL + "mute",
                        {"sink": sink, "source": source, "sourcePort": 0, "mute": blank},
                        self.statusSub,
                        {"audio":[{"sink": sink, "muted": blank}]})

if __name__ == '__main__':
    luna.VERBOSE = False
    unittest.main()