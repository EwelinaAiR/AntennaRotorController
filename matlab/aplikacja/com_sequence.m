dataValues = zeros(1, 1000);
statusValues = zeros(1, 1000);

flushinput(s);
for i = 1:1000
    com_send([10 1]);
    pause(0.1);
    [data, data_size] = com_receive();
    % data
    dane = typecast(data, 'uint16');
   
    dataValues(i) = dane(1);
    statusValues(i) = dane(2);

end    