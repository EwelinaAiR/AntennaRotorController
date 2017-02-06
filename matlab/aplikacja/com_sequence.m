dataValues = zeros(1, 1000);
statusValues = zeros(1, 1000);

flushinput(s);
for i = 1:1000
    
    % send frame
    % command
    order = 1;          % 1 -> closed-loop control
    manualMode = 127;   % max positive number (in mode 1 is neglected)
    desiredAngle = typecast(uint16(100), 'uint8');  % set 100 deg
    
    data = [order manualMode desiredAngle];
    com_send(data);
    pause(0.1);
    [dataRec, data_size] = com_receive();
    
    % analise of the received data
    encoderRawData = typecast(dataRec(1:2), 'uint16');
    status = typecast(dataRec(3:4), 'uint16');
   
    dataValues(i) = encoderRawData;
    statusValues(i) = status;

end    