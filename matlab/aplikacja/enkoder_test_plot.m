N = length(dataValues);
status = [];
for i=1:N
    status(i) = bitand(dataValues(i), 7);
end    

plot(status)