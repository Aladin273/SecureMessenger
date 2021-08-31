TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += Common MessengerLib MessengerLibTest Messenger

MessengerLib.file = MessengerLib/MessengerLib.pro
MessengerLibTest.file = MessengerLibTest/MessengerLibTest.pro
Messenger.file = Messenger/Messenger.pro
Common.file = ../Common/Common.pro
