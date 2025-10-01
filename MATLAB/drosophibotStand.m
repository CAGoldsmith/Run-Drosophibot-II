%Clarus Goldsmith
%West Virginia University
clear all
close all

load restPosture.mat
    %Convert the thetas from radians into servo bits
    %From radians to deg:
    restPosture = restPosture*180/pi;
    %From deg to bits:
    restPosture = restPosture/.088;
    restPosture = round(restPosture);
    restPosture = restPosture + 2048;

    numJoints = 22;
%%

lib_name = '';

if strcmp(computer, 'PCWIN')
    lib_name = 'dxl_x86_c';
elseif strcmp(computer, 'PCWIN64')
    lib_name = 'dxl_x64_c';
elseif strcmp(computer, 'GLNX86')
    lib_name = 'libdxl_x86_c';
elseif strcmp(computer, 'GLNXA64')
    lib_name = 'libdxl_x64_c';
elseif strcmp(computer, 'MACI64')
    lib_name = 'libdxl_mac_c';
end

% Load Libraries
if ~libisloaded(lib_name)
    [notfound, warnings] = loadlibrary(lib_name, 'dynamixel_sdk.h', 'addheader', 'port_handler.h', 'addheader', 'packet_handler.h', 'addheader', 'group_sync_write.h', 'addheader', 'group_sync_read.h');
end

% Control table address
ADDR_TORQUE_ENABLE          = 64;                 % Control table address is different in Dynamixel model
ADDRGOAL_POSITION           = 116;

% Data Byte Length
LEN_GOAL_POSITION       = 4;

% Protocol version
PROTOCOL_VERSION            = 2.0;          % See which protocol version is used in the Dynamixel

% Default setting
ids = linspace(0,numJoints-1,numJoints);
DXL_ID                     = ids;            % Dynamixel#1 ID: 1
BAUDRATE                    = 1000000;
DEVICENAME                  = 'COM3';       % Check which port is being used on your controller
% ex) Windows: 'COM1'   Linux: '/dev/ttyUSB0' Mac: '/dev/tty.usbserial-*'

TORQUE_ENABLE               = 1;            % Value for enabling the torque
TORQUE_DISABLE              = 0;            % Value for disabling the torque

ESC_CHARACTER               = 'e';          % Key for escaping loop

COMM_SUCCESS                = 0;     
% Communication Success result value
COMM_TX_FAIL                = -1001;        % Communication Tx Failed

% Initialize PortHandler Structs
% Set the port path
% Get methods and members of PortHandlerLinux or PortHandlerWindows
port_num = portHandler(DEVICENAME);

% Initialize PacketHandler Structs
packetHandler();

% Initialize Groupsyncwrite Structs
groupwrite_num = groupSyncWrite(port_num, PROTOCOL_VERSION, ADDRGOAL_POSITION, LEN_GOAL_POSITION);

index = 1;
indexCont = 1;
dxl_comm_result = COMM_TX_FAIL;           % Communication result
dxl_addparam_result = false;              % AddParam result
dxl_getdata_result = false;               % GetParam result
dxl_goal_position = restPosture;

dxl_error = 0;                              % Dynamixel error


% Open port
if (openPort(port_num))
    fprintf('Succeeded to open the port!\n');
else
    unloadlibrary(lib_name);
    fprintf('Failed to open the port!\n');
    input('Press any key to terminate...\n');
    return;
end


% Set port baudrate
if (setBaudRate(port_num, BAUDRATE))
    fprintf('Succeeded to change the baudrate!\n');
else
    unloadlibrary(lib_name);
    fprintf('Failed to change the baudrate!\n');
    input('Press any key to terminate...\n');
    return;
end


% Enable Dynamixel Torques
for i=1:numJoints
    write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID(i), ADDR_TORQUE_ENABLE, TORQUE_ENABLE);
    dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION);
    dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION);
    if dxl_comm_result ~= COMM_SUCCESS
        fprintf('%s\n', getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
    elseif dxl_error ~= 0
        fprintf('%s\n', getRxPacketError(PROTOCOL_VERSION, dxl_error));
    else
        fprintf('Dynamixel #%d has been successfully connected \n', DXL_ID(i));
    end
end

if input('Press any key to continue! (or input e to quit!)\n', 's') == ESC_CHARACTER
end

for i=1:numJoints
    % Add Dynamixel goal position value to the Syncwrite storage
    dxl_addparam_result = groupSyncWriteAddParam(groupwrite_num, DXL_ID(i), typecast(int32(dxl_goal_position(i)), 'uint32'), LEN_GOAL_POSITION);
    if dxl_addparam_result ~= true
        fprintf('[ID:%03d] groupSyncWrite addparam failed', DXL_ID(i));
        return;
    end
end

% Syncwrite goal position
groupSyncWriteTxPacket(groupwrite_num);
dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION);
if dxl_comm_result ~= COMM_SUCCESS
    fprintf('%s\n', getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
end

% Clear syncwrite parameter storage
groupSyncWriteClearParam(groupwrite_num);

if input('Input e to quit!)\n', 's') == ESC_CHARACTER
end

   
%%

% Disable each Dynamixel Torque
for i=1:numJoints
    write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL_ID(i), ADDR_TORQUE_ENABLE, TORQUE_DISABLE);
    dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION);
    dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION);
    if dxl_comm_result ~= COMM_SUCCESS
        fprintf('%s\n', getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
    elseif dxl_error ~= 0
        fprintf('%s\n', getRxPacketError(PROTOCOL_VERSION, dxl_error));
    end
end

% Close port
closePort(port_num); 

% Unload Library
unloadlibrary(lib_name);

