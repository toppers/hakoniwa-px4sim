import pygame
from pygame.locals import *

pygame.init()
pygame.joystick.init()
try:
   joystick = pygame.joystick.Joystick(0)
   joystick.init()
   print('ジョイスティックの名前:', joystick.get_name())
   print('ボタン数 :', joystick.get_numbuttons())
   print("ジョイスティック数 : " + str(joystick.get_numaxes()))
   print("十字キー数 : " + str(joystick.get_numhats()))
except pygame.error:
   print('ジョイスティックが接続されていません')

active = True
while active:
   for e in pygame.event.get():
        if e.type == QUIT:
            active = False

        if e.type == pygame.locals.JOYHATMOTION:
            if e.value[0] > 0:
                print("十字キー 右", e.value[0])
            if e.value[0] < 0:
                print("十字キー 左", e.value[0])
            if e.value[1] > 0:
                print("十字キー 上", e.value[1])
            if e.value[1] < 0:
                print("十字キー 下", e.value[1])
        elif e.type == pygame.locals.JOYAXISMOTION:
            if e.axis == 0 or e.axis == 1:
                if e.value == 0.0 or e.value == 1.0 or e.value == -1.0:
                    print('ジョイスティック ー {0} {1}'.format(e.axis, e.value))
                else:
                    print('ジョイスティック  {0} {1}'.format(e.axis, e.value))
            elif  e.axis == 2 or e.axis == 3:
                if e.value == 0.0 or e.value == 1.0 or e.value == -1.0:
                    print('ジョイスティック ー {0} {1}'.format(e.axis, e.value))
                else:
                    print('ジョイスティック  {0} {1}'.format(e.axis, e.value))
            elif  e.axis == 4 or e.axis == 5:
                if e.value == 0.0 or e.value == 1.0 or e.value == -1.0:
                    print('ジョイスティック ー {0} {1}'.format(e.axis, e.value))
                else:
                    print('ジョイスティック  {0} {1}'.format(e.axis, e.value))
            else:
                print('ジョイスティック(ー)')
        elif e.type == pygame.locals.JOYBUTTONDOWN:
            print('ボタン押す {0}'.format(e.button))
        elif e.type == pygame.locals.JOYBUTTONUP:
            print('ボタン離す {0}'.format(e.button))
