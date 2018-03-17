#!/usr/bin/python2
import random
import unittest
import luna_utils as luna

API_URL = "com.webos.service.avoutput/audio/volume/"

class TestAudioVolumeMethods(luna.TestBase):

    def setUp(self):
        luna.call(API_URL + "muteSpeaker", {"tvSoundOutput": "tv_speaker", "mute": False})
        self.status = luna.subscribe(API_URL + "getStatus", {"tvSoundOutput": "tv_speaker", "subscribe":True})

    def tearDown(self):
        luna.cancelSubscribe(self.status)
        luna.call(API_URL + "muteSpeaker", {"tvSoundOutput": "tv_speaker", "mute": True})

    def test_get_status(self):
        ret = luna.call(API_URL + "getStatus", {})
        self.assertContainsData(ret, {"audioStatus":[{"tvSoundOutput":"tv_speaker", "volume":"_any_"}]})

        ret = luna.call(API_URL + "getStatus", {"tvSoundOutput": "tv_speaker"})
        self.assertContainsData(ret, {"audioStatus": [{"tvSoundOutput": "tv_speaker", "volume": "_any_"}]})

    def test_mute(self):
        for mute in [True, False]:
            self.checkLunaCallSuccessAndSubscriptionUpdate(
                API_URL + "muteSpeaker", {"tvSoundOutput": "tv_speaker", "mute": mute},
                self.status, {"audioStatus":[{"tvSoundOutput":"tv_speaker", "muted":mute}]})

    def test_volume_up(self):
        for i in range(2):
            self.checkLunaCallSuccessAndSubscriptionUpdate(API_URL + "up", {"tvSoundOutput": "tv_speaker"},
                                                           self.status,
                                                           {"audioStatus": [{"tvSoundOutput": "tv_speaker", "volume": "_any_"}]})

    def test_volume_down(self):
        for i in range(2):
            self.checkLunaCallSuccessAndSubscriptionUpdate(API_URL + "down", {"tvSoundOutput": "tv_speaker"},
                                                           self.status,
                                                           {"audioStatus": [{"tvSoundOutput": "tv_speaker", "volume": "_any_"}]})

    def test_set_volume(self):
        curVolume = luna.call(API_URL + "getStatus", {"tvSoundOutput": "tv_speaker"})["audioStatus"][0]["volume"]

        for i in range(20):
            volume = random.randint(0, 100)
            #ensure different volume than current
            if volume == curVolume:
                volume = (volume + 1) % 100

            self.checkLunaCallSuccessAndSubscriptionUpdate(API_URL + "set", {"tvSoundOutput": "tv_speaker", "volume":volume},
                                                           self.status,
                                                           {"audioStatus": [{"tvSoundOutput": "tv_speaker", "volume": volume}]})
            curVolume = volume

    def test_set_invalid_volume(self):
        self.checkLunaCallFailAndNoSubscriptionUpdate(API_URL + "set", {"tvSoundOutput": "tv_speaker", "volume":-1}, self.status)
        self.checkLunaCallFailAndNoSubscriptionUpdate(API_URL + "set", {"tvSoundOutput": "tv_speaker", "volume":101}, self.status)
        self.checkLunaCallFailAndNoSubscriptionUpdate(API_URL + "set", {"tvSoundOutput": "tv_speaker", "volume":True}, self.status)
        self.checkLunaCallFailAndNoSubscriptionUpdate(API_URL + "set", {"tvSoundOutput": "atv_speaker", "volume":1}, self.status)

if __name__ == '__main__':
    luna.VERBOSE = False
    unittest.main()