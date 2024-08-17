import pygame
from pygame.locals import *

pygame.init()
pygame.joystick.init()
try:
   joystick = pygame.joystick.Joystick(0)
   joystick.init()
   print('ジョイスティックの名前:', joystick.get_name())
   print('ボタン数 :', joystick.get_numbuttons())
except pygame.error:
   print('ジョイスティックが接続されていません')

active = True
while active:
   for e in pygame.event.get():
        if e.type == QUIT:
            active = False

        if e.type == pygame.locals.JOYHATMOTION:
            if e.value[0] == 0:
                if e.value[1] == 1:
                    print("十字キー：上")
                elif e.value[1] == 0:
                    print("十字キー：ー")
                else:
                    print("十字キー：下")
            elif e.value[0] == 1:
                if e.value[1] == 0:
                    print("十字キー：右")
                elif e.value[1] == 1:
                    print("十字キー：右上")
                else:
                    print("十字キー：右下")
            else:
                if e.value[1] == 0:
                    print("十字キー：左")
                elif e.value[1] == 1:
                    print("十字キー：左上")
                else:
                    print("十字キー：左下")

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

