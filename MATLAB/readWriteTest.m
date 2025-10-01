clear all
arduinoObj = serialport("COM5",115200);
configureTerminator(arduinoObj,"CR/LF")
flush(arduinoObj)
i=1;
loop = 1;
send = input('Send (Y/N)?: ');
write(arduinoObj,send,"uint8")
data(1,:) = readline(arduinoObj);
ts = tic;
tf = toc(ts);
if tf < 20
    tf = toc(ts);
end
data(2,:) = readline(arduinoObj);
write(arduinoObj,0,"uint8")
clear arduinoObj
% figure
% if send > 0
%     while loop
%         ts = tic;
%         data(i) = readline(arduinoObj);
%         dataNum(i) = str2num(data(i));
%         plot(dataNum)
%         i=i+1;
%         tf = toc(ts);
%         if tf < 20
%             tf = toc(ts);
%         end
%     end
% end