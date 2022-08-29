TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += Common ServerLib ServerLibTest Server

ServerLib.file = ServerLib/ServerLib.pro
ServerLibTest.file = ServerLibTest/ServerLibTest.pro
Server.file = Server/Server.pro
Common.file = ../Common/Common.pro
