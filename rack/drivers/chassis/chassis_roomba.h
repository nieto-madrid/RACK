/*
 * RACK - Robotics Application Construction Kit
 * Copyright (C) 2005-2006 University of Hannover
 *                         Institute for Systems Engineering - RTS
 *                         Professor Bernardo Wagner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Authors
 *      Matthias Hentschel <hentschel@rts.uni-hannover.de>
 *
 */
#ifndef __CHASSIS_ROOMBA_H__
#define __CHASSIS_ROOMBA_H__

#include <main/rack_data_module.h>

#include <main/serial_port.h>
#include <drivers/chassis_proxy.h>
#include <drivers/io_proxy.h>
#include <perception/scan2d_proxy.h>

// define module class
#define MODULE_CLASS_ID     CHASSIS

#define CHASSIS_ROOMBA_ROI_START       0x80
#define CHASSIS_ROOMBA_ROI_BAUD        0x81
#define CHASSIS_ROOMBA_ROI_CONTROL     0x82
#define CHASSIS_ROOMBA_ROI_FULL        0x84
#define CHASSIS_ROOMBA_ROI_CLEAN       0x87
#define CHASSIS_ROOMBA_ROI_DRIVE       0x89
#define CHASSIS_ROOMBA_ROI_MOTORS      0x8A
#define CHASSIS_ROOMBA_ROI_SONG        0x8C
#define CHASSIS_ROOMBA_ROI_PLAY        0x8D
#define CHASSIS_ROOMBA_ROI_SENSORS     0x8E
#define CHASSIS_ROOMBA_ROI_FORCE_DOCK  0x8F

#define CHASSIS_ROOMBA_RADIUS_MAX      2000                     // mm
#define CHASSIS_ROOMBA_DEGREES_PER_MM  360.0 / (258.0 * M_PI)

#define CHASSIS_ROOMBA_POS_WALL_X               110
#define CHASSIS_ROOMBA_POS_WALL_Y               200
#define CHASSIS_ROOMBA_POS_CLIFF_LEFT_X         70
#define CHASSIS_ROOMBA_POS_CLIFF_LEFT_Y         -170
#define CHASSIS_ROOMBA_POS_CLIFF_FRONTLEFT_X    170
#define CHASSIS_ROOMBA_POS_CLIFF_FRONTLEFT_Y    -45
#define CHASSIS_ROOMBA_POS_CLIFF_FRONTRIGHT_X   170
#define CHASSIS_ROOMBA_POS_CLIFF_FRONTRIGHT_Y   45
#define CHASSIS_ROOMBA_POS_CLIFF_RIGHT_X        70
#define CHASSIS_ROOMBA_POS_CLIFF_RIGHT_Y        170
#define CHASSIS_ROOMBA_POS_BUMP_LEFT_X          195
#define CHASSIS_ROOMBA_POS_BUMP_LEFT_Y          -95
#define CHASSIS_ROOMBA_POS_BUMP_RIGHT_X         195
#define CHASSIS_ROOMBA_POS_BUMP_RIGHT_Y         95


typedef struct {
    rack_time_t recordingTime;
    int32_t     bumpAndWheeldrop;
    int32_t     wall;
    int32_t     cliffLeft;
    int32_t     cliffFrontLeft;
    int32_t     cliffFrontRight;
    int32_t     cliffRight;
    int32_t     virtualWall;
    int32_t     motorOvercurrents;
    int32_t     dirtDetectorLeft;
    int32_t     dirtDetectorRight;
    int32_t     remoteControl;
    int32_t     buttons;
    int32_t     distance;
    int32_t     angle;
    int32_t     chargingState;
    int32_t     batteryVoltage;
    int32_t     batteryCurrent;
    int32_t     batteryTemperature;
    int32_t     batteryCharge;
    int32_t     batteryCapacity;
} __attribute__((packed)) chassis_roomba_sensor_data;

// scan_2d data message (use max message size)
typedef struct {
    scan2d_data   data;
    scan_point    point[SCAN2D_POINT_MAX];
} __attribute__((packed)) scan2d_data_msg;

// io data message
typedef struct {
    io_data   data;
    uint8_t   value[20 * 4];                // size of chassis_roomba_sensor_data (without recordingtime)
} __attribute__((packed)) io_data_msg;


/**
 * Hardware driver for vacuum cleaning robot iRobot Roomba.
 *
 * @ingroup modules_chassis
 */
class ChassisRoomba : public RackDataModule {
  private:
    // your values
    int                         serialDev;
    int                         scan2dSys;
    int                         scan2dInst;
    int                         ioSys;
    int                         ioInst;
    int                         motorMainBrush;
    int                         motorVacuum;
    int                         motorSideBrush;
    int                         startDocking;
    SerialPort                  serialPort;

    int                         overcurrentCounter;
    char                        serialBuffer[30];
    rack_time_t                 recordingTimeOld;
    chassis_roomba_sensor_data  sensorData;
    scan2d_data_msg             scan2dDataMsg;
    io_data_msg                 ioDataMsg;
    RackMutex                   hwMtx;

    // mailboxes
    RackMailbox                 workMbx;

    // proxies
    Scan2dProxy                 *scan2d;
    IoProxy                     *io;

    uint32_t                    activePilot;
    uint32_t                    scan2dMbxAdr;
    uint32_t                    ioMbxAdr;
    int                         speed;
    float                       omega;

  protected:

    // -> non realtime context
    int  moduleOn(void);
    void moduleOff(void);
    int  moduleLoop(void);
    int  moduleCommand(RackMessage *msgInfo);

    // -> non realtime context
    void moduleCleanup(void);

    int setBaudrate(int baudNum);
    int setMode(int mode);
    int playNote(int songNum, int note, int duration);
    int playSong(int songNum);
    int setCleaningMotor(int mainBrush, int vacuum, int sideBrush);
    int sendMoveCommand(int speed, float omega);
    int readSensorData(chassis_roomba_sensor_data *sensor);
    void createScan2d(chassis_roomba_sensor_data *sensor, scan2d_data *scan2d);

  public:

    // constructor und destructor
    ChassisRoomba();
    ~ChassisRoomba() {};

    // -> non realtime context
    int  moduleInit(void);
};

#endif // __CHASSIS_ROOMBA_H__
