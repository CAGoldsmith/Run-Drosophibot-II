clear all
arduinoObj = serialport("COM4",115200);
configureTerminator(arduinoObj,"CR/LF");
flush(arduinoObj);
write(arduinoObj,1,"uint8");
tf = zeros(1,5);
for i=1:5
    ts = tic;
    strainDataRaw(i,:) = readline(arduinoObj);
    tf(i) = toc(ts);
end
write(arduinoObj,0,"uint8");
clear arduinoObj